import ast
import configparser
import json
import traceback
import urllib.request
from typing import Optional
from pathlib import Path

from flask import Flask, request, render_template

import persistence

app = Flask(__name__)
config = None
pers: persistence.Persistence = persistence.Persistence()


def get_config():
    global config
    if config is None:
        parser = configparser.ConfigParser()
        parser.read('/usr/share/sdos/config.ini')
        defaults = parser['DEFAULT']
        config = {
            'require_auth': defaults.getboolean('RequireAuth'),
            'management_ip': defaults['ManagementIP'],
            'fallback_ip': defaults['FallbackIP'],
            'cloud_fallback': defaults.getboolean('CloudFallback'),
            'cloud_url': defaults['CloudURL'],
        }
        if Path('/usr/share/sdos/override').is_file():
            with open('/usr/share/sdos/override') as f:
                config['override'] = f.read()
    return config


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


def get_url(number=0) -> str:
    conf = get_config()
    url = 'http://'
    if 'override' in conf:
        number -= 1

    if number == -1:
        url += conf['override']
    elif number == 1:
        url += conf['fallback_ip']
    elif number == 2 and conf['cloud_fallback']:
        url += conf['cloud_url']
    else:
        url += conf['management_ip']
    url += ':5006/manage'
    params = request.args.to_dict()
    entries = []
    for key, val in params.items():
        entries.append(str(key) + '=' + str(val))
    url += '?' + ('&'.join(entries))
    return url


@app.route('/', methods=['POST', 'PUT', 'GET'])
def invoke():
    for try_num in range(3):
        try:
            req = urllib.request.Request(url=get_url(try_num), method=request.method, data=request.data,
                                         headers=get_headers())
            resp = urllib.request.urlopen(req, timeout=5)
            resp_string = resp.read().decode("utf-8")
            return resp_string
        except:
            traceback.print_exc()
    return 'error'


@app.route('/view', methods=['GET'])
def render_list():
    res = invoke()
    try:
        res = json.loads(res)
        return render_template('detail.html', item=res)
    except:
        res = ast.literal_eval(res)
        return render_template('list.html', list=res, col=get_collection_name())


@app.route('/manage', methods=['POST', 'PUT'])
def manage_persist():
    print('called manage POST from', request.remote_addr)
    return pers.persist(get_collection_name(), get_data())


@app.route('/manage', methods=['GET'])
def manage_query():
    print('called manage GET from', request.remote_addr)
    data_id = get_id()
    if get_load_data() or data_id is not None:
        return pers.retrieve(get_collection_name(), data_id=get_id())
    else:
        return pers.retrieve_list(get_collection_name())


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5006)
