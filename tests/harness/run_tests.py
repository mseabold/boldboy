import argparse
import subprocess
import json
import os

RUNNERS = os.path.abspath('../../out/tests/runners')


REPORT_HEADER = '''\
<html>
<head>
<link rel="preconnect" href="https://fonts.gstatic.com">
<link href="https://fonts.googleapis.com/css2?family=Inter&display=swap" rel="stylesheet"> 
<title>Boldboy Test Report</title>
<style>
  body {
      font-family: 'Inter', sans-serif;
  }
  table, td {
      border: 1px solid black;
      border-collapse: collapse;
  }
  td {
      padding: 10px;
  }
  .pass {
      background-color: white;
      color: #32a852;
  }
  .fail {
      background-color: white;
      color: #d64242;
  }
</style>
</head>
<body>
'''

REPORT_FOOTER = '''\
</body>
</html>
'''


if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument('test_config')
    parser.add_argument('tests_dir')
    parser.add_argument('-r','--report',default=os.devnull)

    args = parser.parse_args()

    with open(args.test_config, 'r') as config_file, open(args.report, 'w+') as report_file:
        config = json.load(config_file)

        report_file.write(REPORT_HEADER)

        for suite in config:
            print('Running Suite: {}'.format(suite['name']))
            report_file.write('<h2>{}</h2>\n<table>\n'.format(suite['name']))

            for test_name in suite['tests']:
                test = '{}/{}/{}'.format(args.tests_dir, suite['directory'], test_name)
                report_file.write('<tr><td>{}</td>'.format(test_name))
                try:
                    print('Running test: {} ({})... '.format(test_name, test), end='')
                    ret = subprocess.run([RUNNERS + '/' + suite['runner'], test], timeout=120, capture_output=True)
                    if ret.returncode == 0:
                        status = 'Pass'
                    else:
                        status = 'Fail'
                except subprocess.TimeoutExpired:
                    status = 'Timeout'

                print(status)

                report_file.write('<td class="{}">{}</td></tr>\n'.format(status.lower(), status))

            report_file.write('</table>\n')

        report_file.write(REPORT_FOOTER)



