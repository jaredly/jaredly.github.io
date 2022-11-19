set -e
echo "Converting NM to HTML w/ highlighting"
node ~/clone/sites/explorable-code/js/notablemind/convertNm.js a-reason-react-tutorial.nm.txt a-reason-react-tutorial.html a-reason-react-tutorial.sandbox.json /assets/a-reason-react-tutorial "2017-07-05 20:06:00"
echo "Compiling and typechecking code snippets"
node ~/clone/sites/explorable-code/js/executeSandbox.js a-reason-react-tutorial.sandbox.json ../../playgrounds/a-reason-react-tutorial ../assets/a-reason-react-tutorial
rm a-reason-react-tutorial.sandbox.json
