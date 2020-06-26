import uuid
import time
import json
from pathlib import Path


class Persistence:
    __first_call = True
    __store = {}

    def __init__(self):
        pass

    def __init_storage(self, storage: str):
        Path('./storage').mkdir(exist_ok=True)
        if not Path('./storage/ ' + storage + '.json').is_file():
            with open('./storage/ ' + storage + '.json', 'w') as f:
                entry = {
                    'created_at': str(int(round(time.time() * 1000))),
                    'data': {},
                }
                f.write(json.dumps(entry))
        with open('./storage/ ' + storage + '.json', 'r') as f:
            self.__store[storage] = json.load(f)
            print(self.__store[storage])

    def __write_to_storage(self, storage: str, data_id: str, data: dict):
        self.__store[storage]['data'][data_id] = data
        with open('./storage/ ' + storage + '.json', 'w') as f:
            f.write(json.dumps(self.__store[storage]))

    def persist(self, storage: str, data: dict, data_id: str = None) -> str:
        if storage not in self.__store:
            self.__init_storage(storage)
        if data_id is None or data_id == '':
            data_id = str(uuid.uuid4())
        print('Persisting data with id', data_id)
        self.__write_to_storage(storage, data_id, data)

        return data_id

    def retrieve_list(self, storage: str) -> dict:
        return {
            'ids': list(self.retrieve(storage)['data'].keys())
        }

    def retrieve(self, storage: str, data_id: str = None, query=None) -> dict:
        print("Retrieving " + str(data_id) + " from storage " + str(storage))
        if query is None:
            query = {}
        if storage not in self.__store:
            self.__init_storage(storage)
        if data_id is not None and data_id != '':
            if data_id not in self.__store[storage]['data']:
                return {}
            else:
                return self.__store[storage]['data'][data_id]
        ret = self.__store[storage]
        return self.__store[storage]
