set -e
node ~/clone/experiments/explorable-code/js/notablemind/convertNm.js a-reason-react-tutorial.nm a-reason-react-tutorial.html a-reason-react-tutorial.sandbox.json /assets/a-reason-react-tutorial
node ~/clone/experiments/explorable-code/js/executeSandbox.js a-reason-react-tutorial.sandbox.json ../../playgrounds/a-reason-react-tutorial ../assets/a-reason-react-tutorial
