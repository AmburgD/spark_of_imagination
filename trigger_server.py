import json
import os
import socket
import threading
import time

from flask import Flask
from werkzeug.serving import make_server

app = Flask(__name__)

# --- Constants ---
IDLE_VIDEO_PATH = "/home/projectorcomp/idle.gif"
MAIN_VIDEO_PATH = "/home/projectorcomp/prb3Capstonee.gif"
MPV_SOCKET = "/tmp/mpvsocket"


# --- MPV Control Functions ---
def send_mpv_command(command):
    """Send a JSON command to mpv via its IPC socket."""
    try:
        with socket.socket(socket.AF_UNIX) as sock:
            sock.connect(MPV_SOCKET)
            sock.send((json.dumps(command) + "\n").encode("utf-8"))
    except Exception as e:
        print(f"Error sending command to mpv: {e}")


def play_main_video():
    print("Playing main video...")
    send_mpv_command({"command": ["loadfile", MAIN_VIDEO_PATH, "replace"]})
    send_mpv_command({"command": ["set_property", "loop", "no"]})


def resume_idle():
    print("Resuming idle gif...")
    send_mpv_command({"command": ["loadfile", IDLE_VIDEO_PATH, "replace"]})
    send_mpv_command({"command": ["set_property", "loop", "yes"]})


# --- Flask Routes ---
@app.route("/")
def hello():
    return "Hello from Flask!"


@app.route("/play", methods=["POST"])
def trigger_play():
    play_main_video()
    return "Main video started."


@app.route("/stop", methods=["POST"])
def stop_video():
    resume_idle()
    return "Idle gif resumed."


# --- Flask Server in Background Thread ---
class ServerThread(threading.Thread):
    def __init__(self, app):
        threading.Thread.__init__(self)
        self.server = make_server("0.0.0.0", 5000, app)
        self.ctx = app.app_context()
        self.ctx.push()

    def run(self):
        print("Starting Flask server on port 5000...")
        self.server.serve_forever()

    def shutdown(self):
        print("Shutting down Flask server...")
        self.server.shutdown()


# --- Start mpv in idle mode with IPC ---
def start_mpv_idle():
    print("Starting mpv in idle mode...")
    # Remove stale socket if it exists
    if os.path.exists(MPV_SOCKET):
        os.remove(MPV_SOCKET)

    subprocess.Popen(["mpv", "--idle=yes", "--fs", "--osd-level=0", "--input-ipc-server=" + MPV_SOCKET, "--loop", IDLE_VIDEO_PATH])


# --- Main ---
if __name__ == "__main__":
    import subprocess

    start_mpv_idle()

    flask_thread = ServerThread(app)
    flask_thread.start()

    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("Shutting down gracefully...")
        flask_thread.shutdown()
