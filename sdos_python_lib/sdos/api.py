import json
import traceback
import urllib.request

__port: int = 5006


def __get_url(collection: str, id: str = None) -> str:
    if id is None:
        return 'http://localhost:' + str(__port) + '?collection=' + collection
    else:
        return 'http://localhost:' + str(__port) + '?collection=' + collection + '&id=' + id


def entries(collection: str) -> [str]:
    try:
        req = urllib.request.Request(url=__get_url(collection), method='GET')
        resp = urllib.request.urlopen(req)
        resp_obj = json.loads(resp.read().decode("utf-8"))
        return resp_obj['ids']
    except:
        traceback.print_exc()
        return None


def get(collection: str, id: str, example=None) -> dict:
    if example is None:
        example = {}
    example = json.dumps(example).encode('utf-8')
    try:
        req = urllib.request.Request(url=__get_url(collection, id), data=example, method='GET')
        resp = urllib.request.urlopen(req)
        resp_obj = json.loads(resp.read().decode("utf-8"))
        return resp_obj
    except:
        traceback.print_exc()


def post(collection: str, data:dict, id: str = None) -> str:
    data = json.dumps(data).encode('utf-8')
    try:
        req = urllib.request.Request(url=__get_url(collection, id), data=data, method='POST')
        resp = urllib.request.urlopen(req)
        resp_obj = json.loads(resp.read().decode("utf-8"))
        return resp_obj
    except:
        traceback.print_exc()
