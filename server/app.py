from distutils.log import debug
import os
import time
import json
import sqlite3
from flask import Flask, g, request
import flask_socketio as sio
from httplogging import LoggingMiddleware

app = Flask(__name__)
app.config['SECRET_KEY'] = 'My super secret secret'

DATABASE = os.path.join(os.getcwd(), 'database.db')

socketio = sio.SocketIO(app, cors_allowed_origins=['http://localhost:8080', 'http://192.168.2.31:8080', 'https://hoog3059.pythonanywhere.com'])


@app.get('/api')
def api_get():
    data = get_data()
    return data


@app.post('/api')
def api_post():
    data = request.get_json()
    data['timestamp'] = int(time.time() * 1000)
    db_data = get_data()
    db_data.update(data)
    update_data(db_data)
    socketio.emit("data_update", db_data)  # Broadcast
    return "OK", 200


@socketio.on('connect')
def new_connection(auth):
    sio.emit("data_update", get_data())


def get_data():
    return json.loads(query_db("select data from datatable where id = 1;", one=True)[0])


def update_data(data):
    query_db("update datatable set data = ? where id = 1;", (json.dumps(data),))


def get_db():
    db = getattr(g, '_database', None)
    if db is None:
        db = g._database = sqlite3.connect(DATABASE)
    return db


def query_db(query, args=(), one=False):
    db = get_db()
    cur = db.execute(query, args)
    rv = cur.fetchall()
    cur.close()
    db.commit()
    return (rv[0] if rv else None) if one else rv


@app.teardown_appcontext
def close_connection(exception):
    db = getattr(g, '_database', None)
    if db is not None:
        db.close()


if __name__ == '__main__':
    print(f"Cwd: {os.getcwd()}")
    print(f"Database loaded: {DATABASE}")
    # app.wsgi_app = LoggingMiddleware(app.wsgi_app)
    socketio.run(app, host="0.0.0.0", port=5000, debug=True)
