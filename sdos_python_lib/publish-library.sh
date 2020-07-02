# publish python library as pip package
API_KEY=$(<api-key)
rm -rf dist
python3 setup.py sdist bdist_wheel
twine upload --verbose --repository testpypi --non-interactive dist/* -u "__token__"  -p "$API_KEY"