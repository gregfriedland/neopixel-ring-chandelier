#!python

import sys
import urllib2
import pycurl
import colorsys
from io import BytesIO
import time

def parse_colour_lovers(data):
    import json

    palettes = []
    data = json.loads(data)
    for entry in data:
        palettes.append(map(lambda hex: int(hex, 16), entry["colors"]))

    return palettes

def parse_kuler(data):
    import xmltodict

    palettes = []
    data = xmltodict.parse(data)
    for item in data["rss"]["channel"]["item"]:
        palette = []
        for hexColor in item["kuler:themeItem"]["kuler:themeSwatches"]["kuler:swatch"]:
            hexColor = hexColor["kuler:swatchHexColor"]
            palette.append(int(hexColor, 16))
        palettes.append(palette)

    #print palettes
    return palettes
    

if __name__ == "__main__":
    service = sys.argv[1] # kuler or colour-lovers
    npages = int(sys.argv[2])

    all_palettes = []
    for i in range(npages):
        if service == "kuler":
            import ConfigParser
            
            config = ConfigParser.ConfigParser()
            config.read('local.cfg')
            key = config.get("Keys", "kuler")

            url = "https://kuler-api.adobe.com/rss/get.cfm?listtype=rating&itemsPerPage=100&startIndex=%d&key=%s" % (i*100, key)
        elif service == "colour-lovers":
            url = "http://www.colourlovers.com/api/palettes/top?format=json&numResults=100&resultOffset=%d" % (i * 100)
        else:
            print "Invalid service: " + service
            sys.exit(1)

        print >> sys.stderr, url
        buffer = BytesIO()
        curl = pycurl.Curl()
        curl.setopt(pycurl.URL, url)
        curl.setopt(curl.WRITEDATA, buffer)
        curl.setopt(pycurl.USERAGENT, 'Mozilla/5.0 (Windows NT 6.3; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/37.0.2049.0 Safari/537.36')
        curl.perform()
        curl.close()

        # print buffer.getvalue()

        if service == "kuler":
            palettes = parse_kuler(buffer.getvalue())
        elif service == "colour-lovers":
            palettes = parse_colour_lovers(buffer.getvalue())

        for palette in palettes:
            rgbs = [((col >> 16) & 0xFF, (col >> 8) & 0xFF, col & 0xFF) for col in palette]
            rgbs = [(rgb[0]/255.0, rgb[1]/255.0, rgb[2]/255.0) for rgb in rgbs]
            hsvs = [colorsys.rgb_to_hsv(rgb[0], rgb[1], rgb[2]) for rgb in rgbs]
            tot_sat = sum([hsv[1] for hsv in hsvs])
            #print rgbs[0], hsvs[0]
            all_palettes += [(tot_sat, palette)]
            #print rgbs, " ".join(["%.2f" % hsv[1] for hsv in hsvs])
            #print ",".join(["0x%x"%col for col in cols])

        # don't try to fetch them too fast
        time.sleep(1)

    for tot_sat, palette in reversed(sorted(all_palettes)):
        if len(palette) == 5:
            print ",".join(["0x%x"%col for col in palette]) +","
