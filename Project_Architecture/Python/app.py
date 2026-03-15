import serial
import json
import time
import threading
from flask import Flask, jsonify, render_template

app = Flask(__name__)

latest_data = {
    "temp": 0,
    "hum": 0,
    "mic": 0,
    "ax": 0,
    "ay": 0,
    "az": 0
}

def read_serial():
    global latest_data

    print("Opening serial...")
    ser = serial.Serial("COM8", 115200, timeout=1)
    time.sleep(2)
    print("Serial opened.")

    while True:
        try:
            line = ser.readline().decode(errors="ignore").strip()

            if not line:
                continue

            print("RAW:", line)

            if not line.startswith("{"):
                continue

            data = json.loads(line)

            latest_data = {
                "temp": data.get("temp", 0),
                "hum": data.get("hum", 0),
                "mic": data.get("mic", 0),
                "ax": data.get("ax", 0),
                "ay": data.get("ay", 0),
                "az": data.get("az", 0),
            }

            print("UPDATED:", latest_data)

        except Exception as e:
            print("Serial error:", e)

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/data")
def data():
    print("Serving:", latest_data)
    return jsonify(latest_data)

if __name__ == "__main__":
    t = threading.Thread(target=read_serial, daemon=True)
    t.start()
    app.run(debug=True, use_reloader=False)
