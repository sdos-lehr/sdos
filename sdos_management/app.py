import ast
import json
import traceback
import urllib.request
from pathlib import Path
from typing import Optional

from flask import Flask, request, render_template, send_from_directory
from flask_cors import CORS, cross_origin

import persistence

app = Flask(__name__, static_folder='/usr/share/sdos/static')
cors = CORS(app)
app.config['CORS_HEADERS'] = 'Content-Type'
ip = None
pers: persistence.Persistence = persistence.Persistence()


def get_ip():
    global ip
    if ip is None:
        if Path('/usr/share/sdos/management-ip').is_file():
            with open('/usr/share/sdos/management-ip') as f:
                ip = f.read()
    return ip


def get_param(param: str) -> Optional:
    data = request.args
    if param in data:
        return data[param]
    return None


def get_collection_name():
    return get_param('collection')


def get_id() -> Optional[str]:
    return get_param('id')


def get_load_data() -> Optional[bool]:
    return get_param('load_data')


def get_headers() -> dict:
    return dict(request.headers)


def get_data() -> dict:
    if request.is_json:
        return dict(request.json)
    else:
        data = json.loads(request.data.decode('utf-8'))
        return data


def get_url() -> str:
    ip = get_ip()
    url = 'http://' + str(ip) + ':5006/manage'

    params = request.args.to_dict()
    entries = []
    for key, val in params.items():
        entries.append(str(key) + '=' + str(val))
    url += '?' + ('&'.join(entries))
    return url


@app.route('/', methods=['POST', 'PUT', 'GET'])
@cross_origin()
def invoke():
    try:
        agent = request.headers.get('User-Agent')
        if 'mozilla' in agent.lower() and (not request.args or len(request.args) == 0):
            print('returning index.html')
            return send_from_directory(app.static_folder, 'index.html')
    except:
        traceback.print_exc()
    for try_num in range(2):
        try:
            req = urllib.request.Request(url=get_url(), method=request.method, data=request.data,
                                         headers=get_headers())
            resp = urllib.request.urlopen(req, timeout=5)
            resp_string = resp.read().decode("utf-8")
            return resp_string
        except:
            traceback.print_exc()
    return 'error'


@app.route('/manage', methods=['POST', 'PUT'])
@cross_origin()
def manage_persist():
    print('called manage POST from', request.remote_addr)
    return pers.persist(get_collection_name(), get_data(), data_id=get_id())


@app.route('/manage', methods=['GET'])
@cross_origin()
def manage_query():
    print('called manage GET from', request.remote_addr)
    data_id = get_id()
    if get_load_data() or data_id is not None:
        return pers.retrieve(get_collection_name(), data_id=get_id())
    else:
        return pers.retrieve_list(get_collection_name())


@app.route('/<path:path>', methods=['GET'])
@cross_origin()
def static_proxy(path: str):
    if path.endswith('favicon.ico'):
        return send_from_directory(app.static_folder, 'favicon.ico')
    if not Path(app.static_folder + '/' + path).is_file():
        return send_from_directory(app.static_folder, 'index.html')
    return send_from_directory(app.static_folder, path)


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5006)
