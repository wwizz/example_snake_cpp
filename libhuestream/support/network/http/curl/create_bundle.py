#!/usr/bin/env python

import requests

r = requests.get('https://curl.haxx.se/ca/cacert.pem')
certlist = r.text.split('\n\n')[1:]

with open('CABundle.txt', 'w') as f:
    for cert in certlist:
        # eliminate any non-ascii character
        s = ''.join([x for x in cert if ord(x) in range(128)])
        f.write('R"(%s)",\n' % s)
