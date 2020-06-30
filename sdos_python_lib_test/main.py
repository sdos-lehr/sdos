import sdos.api

ids = sdos.api.entries('writing_demo')
for id in sdos.api.entries('writing_demo'):
    print(id)
    value = sdos.api.get('writing_demo', id)
    print(value)
    print(value['demo_value'] + 1)