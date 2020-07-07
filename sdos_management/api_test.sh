json_res=$(wget -qO - http://localhost:5006?collection=writing_demo)
ids=$(echo $json_res | jq '.ids' | tr -cd '0-9a-z\-\n')

for id in $ids
do
  echo $id':'
  entry=$(wget -qO - http://localhost:5006?collection=writing_demo\&id=$id)
  echo $entry | jq '.'
  break
done