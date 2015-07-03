import sys, pygame, time, random
clock = pygame.time.Clock()

NUM_LEDS = 60
PALETTE_SIZE = 1024
MAX_POSSIBLE_SPEED = 5000
MAX_ACCELERATION = 256
FPS = 200
POS_PRECISION = 200

colorTable = [0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00, 0x00FFFF, 0xFF0000,
              0x690011, 0xBF0426, 0xCC2738, 0xF2D99C, 0xE5B96F, 0x690011,
              0x04BFBF, 0xCAFCD8, 0xF7E967, 0xA9CF54, 0x588F27, 0x04BFBF,
              0x63A69F, 0xF2E1AC, 0xF2836B, 0xF2594B, 0xCD2C24, 0x63A69F]

NUM_COLORS_PER_PALETTE = 6
NUM_PALETTES = len(colorTable) / NUM_COLORS_PER_PALETTE

def  map(x, in_min, in_max, out_min, out_max):
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min

def colorLookup(index):
    return colorTable[index]
  
def colorToRgb(col, rgb):
    rgb[0] = (col >> 16) & 255
    rgb[1] = (col >> 8) & 255
    rgb[2] = col & 255

def interp16(start, end, index, length):
    if index >= length:
        return end
    return start + index*(end-start)/length

def getGradientColor(palIndex, gradientIndex, gradientSize):
    gradientIndex = (gradientIndex + gradientSize) % gradientSize

    subGradientSize = gradientSize / NUM_COLORS_PER_PALETTE

    colIndex1 = gradientIndex / subGradientSize
    colIndex2 = (colIndex1 + 1) % NUM_COLORS_PER_PALETTE
  
    rgb1, rgb2 = [0,0,0], [0,0,0]
    colorToRgb(colorLookup(palIndex * NUM_COLORS_PER_PALETTE + colIndex1), rgb1)
    colorToRgb(colorLookup(palIndex * NUM_COLORS_PER_PALETTE + colIndex2), rgb2)

    subGradientIndex = gradientIndex % subGradientSize

    #print "gradInd=%d subGradInd=%d colInd1/2=%d/%d" % (gradientIndex, subGradientIndex, colIndex1, colIndex2)
    #print "gradSize=%d subGradSize=%d" % (gradientSize, subGradientSize)

    rgb = [0,0,0]
    rgb[0] = interp16(rgb1[0], rgb2[0], subGradientIndex, subGradientSize)
    rgb[1] = interp16(rgb1[1], rgb2[1], subGradientIndex, subGradientSize)
    rgb[2] = interp16(rgb1[2], rgb2[2], subGradientIndex, subGradientSize)
    #print "rgb1=%s rgb2=%s rgb=%s" % (rgb1, rgb2, rgb)

    return rgb

class Palette:
    def __init__(self, palIndex, palSize):
        self.palIndex = palIndex
        self.palSize = palSize
    
    def getColor(self, colIndex):
        return getGradientColor(self.palIndex, colIndex, self.palSize)

class PatternSettings:
    def __init__(self, *args):
        self.numLeds, self.initSpeed, self.maxSpeed, self.minSpeed, self.acceleration, self.colIncrement, self.eventProb, \
            self.eventLength, self.groupSize = args
        

class PatternState:
    def __init__(self, settings):
        self.settings, self.pos, self.currColorIndex, self.currSpeed, self.currAcceleration = settings, 0, 0, settings.initSpeed, settings.acceleration
        
    def direction(self):
        return 1 if self.currSpeed >= 0  else -1
  
    # def currDelay(self):
    #     delay = map(abs(self.currSpeed), 0, MAX_POSSIBLE_SPEED, 100, 1)
    #     print "speed=%4d delay=%4d" % (self.currSpeed, delay)
    #     delay = min(max(delay, 1), 1000)
    #     return delay
  
    def update(self):
        accelDirection = 1 if random.random() > 0.5 else -1
        newSpeed = self.currSpeed * (MAX_ACCELERATION + accelDirection * self.currAcceleration) / 256
        newSpeed = max(min(newSpeed, self.settings.maxSpeed), self.settings.minSpeed)
          
        self.currSpeed = newSpeed
        self.currColorIndex += self.settings.colIncrement
        print "currSpeed=%4d currAccel=%4d colIndex=%5d" % (self.currSpeed, self.currAcceleration, self.currColorIndex)


class Pattern:
    def __init__(self, leds, palette, settings):
        self.leds, self.palette, self.state = leds, palette, PatternState(settings)
    
    def setPalette(self, palette):
        self.palette = palette

    def gradient(self, isWave=False, singleWave=False):
        waveSize = self.state.settings.groupSize

        for i in range(self.state.settings.numLeds):
            colIndex = (self.state.pos / POS_PRECISION + self.state.direction() * i) * self.state.settings.colIncrement
            col = self.palette.getColor(colIndex)
            #if i == 0: print "i=%d currLed=%d colIndex=%d col=%s" % (i, self.state.currLed, colIndex, col)
            self.leds[i] = col
    
        if isWave:
            waveIndex = self.state.pos / POS_PRECISION + i
            if waveIndex >= waveSize and singleWave:
                waveVal = 0
            else:
                waveVal = sin((waveIndex % waveSize) * 3.1415926 / waveSize)

            # scale intensity with number between 0-255
            self.leds[i] = self.leds[i] * waveVal

  
        # FIX: make sure we wrap around smoothly?
        fastLED.show(self.leds)
        # currDelay = self.state.currDelay()
        fastLED.delay(1000/FPS)
        self.state.update()
        self.state.pos += self.state.currSpeed

class FastLED:
    def __init__(self):
        pygame.init()
        pygame.display.init()
        self.size = (1000,480)
        self.screen = pygame.display.set_mode(self.size)
        self.ledSize = self.size[0] / NUM_LEDS * 3 / 4

    def show(self, leds):
        self.screen.fill((0,0,0))
        pygame.draw.circle(self.screen, (50,0,0), (self.size[0]/2,self.size[1]/2), 200, 0)
        for i, col in enumerate(leds):
            pygame.draw.circle(self.screen, col, (i*self.size[0] / NUM_LEDS ,self.size[1]/2), self.ledSize/2, 0)
        pygame.display.flip()
        
    def delay(self, delayMs):
        clock.tick(1000 / delayMs)
        #time.sleep(delayMs / 1000.0)

fastLED = FastLED()

if __name__ == "__main__":
    gradient = Pattern([0]*NUM_LEDS, Palette(2, PALETTE_SIZE), PatternSettings(NUM_LEDS, 250, 5000, 250, 25, PALETTE_SIZE / NUM_LEDS / 2, 0, 0, 0))
    while True:
        gradient.gradient()

