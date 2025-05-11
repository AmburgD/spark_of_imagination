import subprocess
import threading
import time

from flask import Flask
from werkzeug.serving import make_server

app = Flask(__name__)

IDLE_VIDEO_PATH = "/home/projectorcomp/idle.gif"
MAIN_VIDEO_PATH = "/home/projectorcomp/prb3Capstonee.gif"
video_process = None


# --- Flask Routes ---
@app.route("/")
def hello():
    return "Hello from Flask!"


@app.route("/play", methods=["POST"])
def trigger_play():
    global video_process

    # Stop current playback (idle or main)
    if video_process and video_process.poll() is None:
        video_process.terminate()
        video_process.wait()

    # Start main video in a separate thread
    threading.Thread(target=play_main_video).start()
    return "Main video started."


@app.route("/stop", methods=["POST"])
def stop_video():
    global video_process

    if video_process and video_process.poll() is None:
        print("Stopping video...")
        video_process.terminate()
        video_process.wait()

    # Resume idle
    print("Resuming idle gif...")
    video_process = subprocess.Popen(["mpv", "--loop", "--fs", "--osd-level=0", IDLE_VIDEO_PATH])
    return "Video stopped and idle gif resumed."


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


# --- Play Idle GIF on Startup ---
def play_idle_gif():
    global video_process
    print("Starting idle gif...")
    video_process = subprocess.Popen(["mpv", "--loop", "--fs", "--osd-level=0", IDLE_VIDEO_PATH])


# --- Play Main Video (Non-blocking helper) ---
def play_main_video():
    global video_process
    print("Playing main video...")
    video_process = subprocess.Popen(["mpv", "--fs", "--osd-level=0", MAIN_VIDEO_PATH])
    video_process.wait()

    # When video ends naturally, resume idle gif
    print("Main video ended, resuming idle gif...")
    video_process = subprocess.Popen(["mpv", "--loop", "--fs", "--osd-level=0", IDLE_VIDEO_PATH])


# --- Main ---
if __name__ == "__main__":
    play_idle_gif()

    flask_thread = ServerThread(app)
    flask_thread.start()

    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("Shutting down gracefully...")
        flask_thread.shutdown()
        if video_process and video_process.poll() is None:
            video_process.terminate()
            video_process.wait()
