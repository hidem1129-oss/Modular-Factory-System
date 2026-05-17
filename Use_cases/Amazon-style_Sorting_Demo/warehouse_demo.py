import cv2
import numpy as np
import time
import subprocess

BUS = 1

DC_ADDR = "0x14"
SENSOR_ADDR = "0x15"
GATE_SERVO_ADDR = {
    "blue": "0x13",
    "green": "0x16",
    "yellow": "0x17",
}

GATE_OPEN_DEG = 90
GATE_PUSH_DEG = 180
GATE_HOLD_SEC = 0.5

THRESHOLD = 3600
COOLDOWN_SEC = 1.0
CAPTURE_DELAY_SEC = 1.8

CAMERA_ID = 0

GATE_CALIB_DEG = 0
GATE_CALIB_WAIT_SEC = 0.3

last_gate_addr = None


def i2cset(addr, reg, val):
    subprocess.run(
        ["i2cset", "-y", str(BUS), addr, reg, val],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
        check=True,
    )


def i2cget(addr, reg):
    r = subprocess.run(
        ["i2cget", "-y", str(BUS), addr, reg],
        stdout=subprocess.PIPE,
        stderr=subprocess.DEVNULL,
        text=True,
        check=True,
    )
    return r.stdout.strip()


def start_dc_conveyor():
    i2cset(DC_ADDR, "0x31", "0x01")
    i2cset(DC_ADDR, "0x32", "0x35")
    i2cset(DC_ADDR, "0x35", "0xFF")
    i2cset(DC_ADDR, "0x36", "0xFF")
    i2cset(DC_ADDR, "0x1D", "0x02")

    i2cset(DC_ADDR, "0x30", "0x10")
    i2cset(DC_ADDR, "0x30", "0x00")
    i2cset(DC_ADDR, "0x30", "0x01")


def read_sensor():
    i2cset(SENSOR_ADDR, "0x1D", "0x02")
    i2cset(SENSOR_ADDR, "0x30", "0x10")
    i2cset(SENSOR_ADDR, "0x30", "0x00")
    i2cset(SENSOR_ADDR, "0x30", "0x01")

    time.sleep(0.05)

    hi = i2cget(SENSOR_ADDR, "0x20")
    lo = i2cget(SENSOR_ADDR, "0x21")

    hi_dec = int(hi.replace("0x", ""), 16)
    lo_dec = int(lo.replace("0x", ""), 16)

    return (hi_dec << 8) | lo_dec


def move_servo_deg(addr, deg):
    deg = int(deg)
    hi = (deg >> 8) & 0xFF
    lo = deg & 0xFF

    i2cset(addr, "0x31", f"0x{hi:02X}")
    i2cset(addr, "0x32", f"0x{lo:02X}")

    i2cset(addr, "0x3C", "0x00")
    i2cset(addr, "0x3D", "0x05")

    i2cset(addr, "0x1D", "0x00")

    i2cset(addr, "0x30", "0x10")
    i2cset(addr, "0x30", "0x00")
    i2cset(addr, "0x30", "0x01")
    
   
def reset_all_gates():
    for addr in GATE_SERVO_ADDR.values():
        move_servo_deg(addr, GATE_OPEN_DEG)
        time.sleep(0.1)
       
last_gate_addr = None

def activate_gate(result):
    global last_gate_addr

    addr = GATE_SERVO_ADDR.get(result)

    if last_gate_addr is not None:
        move_servo_deg(last_gate_addr, GATE_OPEN_DEG)
        time.sleep(0.15)
        last_gate_addr = None

    if addr is None:
        return None

    move_servo_deg(addr, GATE_OPEN_DEG)
    time.sleep(0.05)
    move_servo_deg(addr, GATE_PUSH_DEG)

    last_gate_addr = addr
    return addr


def detect_color(frame):
    h, w, _ = frame.shape

    x1 = int(w * 0.20)
    x2 = int(w * 0.80)
    y1 = int(h * 0.45)
    y2 = int(h * 0.75)

    crop = frame[y1:y2, x1:x2]
    hsv = cv2.cvtColor(crop, cv2.COLOR_BGR2HSV)

    mask = (hsv[:, :, 1] > 100) & (hsv[:, :, 2] > 80)
    target = hsv[mask]

    if len(target) == 0:
        return "unknown", None, (x1,y1,x2,y2)

    H = float(np.median(target[:, 0]))
    S = float(np.median(target[:, 1]))
    V = float(np.median(target[:, 2]))

    if 100 <= H <= 130:
        result = "blue"
    elif 45 <= H <= 85:
        result = "green"
    elif 18 <= H <= 40:
        result = "yellow"
    else:
        result = "unknown"

    return result, (H,S,V), (x1,y1,x2,y2)


def servo_angle_for_result(result):
    if result == "blue":
        return 30
    if result == "green":
        return 90
    if result == "yellow":
        return 150
    return None


cap = cv2.VideoCapture(CAMERA_ID)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 320)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 240)

if not cap.isOpened():
    raise RuntimeError("camera open failed")

for _ in range(10):
    cap.read()

def calibrate_all_gates():
    print("Servo calibration: move all gates to 0 deg")

    for name, addr in GATE_SERVO_ADDR.items():
        print(f"  {name}: addr={addr} -> {GATE_OPEN_DEG} deg")
        move_servo_deg(addr, GATE_OPEN_DEG)
        time.sleep(0.3)


print("Camera ready.")
print("Calibrating gates...")
calibrate_all_gates()
print("Gates calibrated.")

print("Starting conveyor...")
start_dc_conveyor()
print("Conveyor started.")
print("Waiting for sensor trigger. Ctrl+C to stop.")

try:
    while True:
        sensor_value = read_sensor()

        if sensor_value < THRESHOLD:
            time.sleep(CAPTURE_DELAY_SEC)

            for _ in range(2):
                cap.read()

            ret, frame = cap.read()
            if not ret:
                print("frame read failed")
                time.sleep(COOLDOWN_SEC)
                continue

            start = time.time()
            result, hsv, roi = detect_color(frame)
            x1,y1,x2,y2 = roi
            elapsed = time.time() - start

            gate_addr = activate_gate(result)

            now = time.strftime("%H:%M:%S")

            if hsv is None:
                print(
                    f"{now} SENSOR={sensor_value} "
                    f"RESULT={result} SERVO={angle} elapsed={elapsed:.3f}s"
                )
            else:
                H, S, V = hsv
                print(
                    f"{now} SENSOR={sensor_value} "
                    f"HSV={H:.1f},{S:.1f},{V:.1f} "
                    f"RESULT={result} GATE={gate_addr} "
                    f"elapsed={elapsed:.3f}s"
                )

            time.sleep(COOLDOWN_SEC)
            disp = frame.copy()

            cv2.rectangle(disp, (x1,y1), (x2,y2), (0,0,255), 2)

            text = f"{result}"
            if hsv is not None:
                H,S,V = hsv
                text += f" H={H:.0f} S={S:.0f} V={V:.0f}"

            cv2.putText(
                disp, text,
                (10, 25),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.6,
                (0,255,0),
                2
            )

            cv2.imshow("Warehouse Camera", disp)
            cv2.waitKey(1)

        else:
            time.sleep(0.05)

except KeyboardInterrupt:
    print("\nStopped.")
    i2cset(DC_ADDR, "0x30", "0x00")

finally:
    cap.release()
    cv2.destroyAllWindows()


