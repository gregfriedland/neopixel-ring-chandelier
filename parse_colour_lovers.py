#!python

import sys
import json
import urllib2
import pycurl
import colorsys
from io import BytesIO

if __name__ == "__main__":
    npages = int(sys.argv[1])

    all_cols = []
    for i in range(npages):
        url = "http://www.colourlovers.com/api/palettes/top?format=json&numResults=100&resultOffset=%d" % (i * 100)

        buffer = BytesIO()
        curl = pycurl.Curl()
        curl.setopt(pycurl.URL, url)
        curl.setopt(curl.WRITEDATA, buffer)
        curl.setopt(pycurl.USERAGENT, 'Mozilla/5.0 (Windows NT 6.3; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/37.0.2049.0 Safari/537.36')
        curl.perform()
        curl.close()

        #print buffer.getvalue()
        data = json.loads(buffer.getvalue())
        for entry in data:
            cols = map(lambda hex: int(hex, 16), entry["colors"])
            rgbs = [((col >> 16) & 0xFF, (col >> 8) & 0xFF, col & 0xFF) for col in cols]
            rgbs = [(rgb[0]/255.0, rgb[1]/255.0, rgb[2]/255.0) for rgb in rgbs]
            hsvs = [colorsys.rgb_to_hsv(rgb[0], rgb[1], rgb[2]) for rgb in rgbs]
            tot_sat = sum([hsv[1] for hsv in hsvs])
            #print rgbs[0], hsvs[0]
            all_cols += [(tot_sat, cols)]
            #print rgbs, " ".join(["%.2f" % hsv[1] for hsv in hsvs])
            #print ",".join(["0x%x"%col for col in cols])

    for tot_sat, cols in reversed(sorted(all_cols)):
        if len(cols) == 5:
            print ",".join(["0x%x"%col for col in cols]) +","
