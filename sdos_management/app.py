import configparser
import json
import sys
import traceback
import ast

from flask import Flask, request, render_template
from pip._vendor import requests
import urllib.request

import persistence

app = Flask(__name__)
config = None
pers: persistence.Persistence = persistence.Persistence()


def get_config():
    global config
    if config is None:
        parser = configparser.ConfigParser()
        parser.read('config.ini')
        defaults = parser['DEFAULT']
        config = {
            'require_auth': defaults.getboolean('RequireAuth'),
            'management_ip': defaults['ManagementIP'],
            'fallback_ip': defaults['FallbackIP'],
            'cloud_fallback': defaults.getboolean('CloudFallback'),
            'cloud_url': defaults['CloudURL'],
        }
    return config


def get_collection_name():
    backup = None
    if 'collection' in get_data():
        backup = get_data()['collection']
    return request.values.get('collection', request.values.get('col', request.values.get('c', backup)))


def get_id():
    backup = None
    if 'id' in get_data():
        backup = get_data()['id']
    return request.values.get('data_id', request.values.get('id', backup))


def get_load_data():
    return request.values.get('load_data', False)


def get_headers() -> dict:
    return dict(request.headers)


def get_data() -> dict:
    if request.is_json:
        return dict(request.json)
    else:
        return dict(request.data)


def get_url(number=0) -> str:
    conf = get_config()
    url = 'http://'
    if number == 1:
        url += conf['fallback_ip']
    elif number == 2 and conf['cloud_fallback']:
        url += conf['cloud_url']
    else:
        url += conf['management_ip']
    url += ':5000/manage'
    params = dict(request.values)
    entries = []
    for key, val in params.items():
        entries.append(str(key) + '=' + str(val))
    url += '?' + ('&'.join(entries))
    return url


@app.route('/', methods=['POST', 'PUT', 'GET'])
def invoke():
    print('called invoke with ' + request.method)
    for try_num in range(3):
        try:
            req = urllib.request.Request(url=get_url(try_num), method=request.method, data=request.data, headers=get_headers())
            resp = urllib.request.urlopen(req)
            resp_string = resp.read().decode("utf-8")
            return resp_string
        except:
            traceback.print_exc()
            print('Exception caught at try', try_num)
            print(get_url(try_num))
    return 'error'


@app.route('/view', methods=['GET'])
def render_list():
    res = invoke()
    try:
        res = json.loads(res)
        print(res)
        return render_template('detail.html', item=res)
    except:
        res = ast.literal_eval(res)
        print(res)
        return render_template('list.html', list=res, col=get_collection_name())


@app.route('/manage', methods=['POST', 'PUT'])
def manage_persist():
    return pers.persist(get_collection_name(), get_data())


@app.route('/manage', methods=['GET'])
def manage_query():
    print('called manage')
    if get_load_data() or get_id() is not None:
        return pers.retrieve(get_collection_name(), data_id=get_id())
    else:
        return pers.retrieve_list(get_collection_name())


if __name__ == '__main__':
    app.run(host='0.0.0.0')
