import sdos.api


stress_test_dict = {}
for i in range(10000):
    stress_test_dict['entry_' + str(i)] = 'value ' + str(i)
print('posting 10000 elements large dict')
print(sdos.api.post('stress_test', stress_test_dict))

for entry in sdos.api.entries('stress_test'):
    print('fetching entry', entry)
    print(sdos.api.get('stress_test', entry))

'''
ids = sdos.api.entries('writing_demo')
print(ids)
for id in ids:
    value = sdos.api.get('writing_demo', id)
    print(value)
    print(value['demo_value'] + 1)
'''

# 8:32 : 1
# 8:33 : 2