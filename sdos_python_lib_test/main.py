import sdos.api

print(sdos.api.post('writing_demo', {
    'demo_value': 5, 'demo_string': 'awesomeness'
}))

ids = sdos.api.entries('writing_demo')
print(ids)
for id in ids:
    value = sdos.api.get('writing_demo', id)
    print(value)
    print(value['demo_value'] + 1)