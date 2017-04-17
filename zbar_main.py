# __UNTESTED__CODE__: Code haven't been tested physically yet
# __UNTESTED__LOGIC__: Logic of code haven't been tested
#!/usr/bin/python
import sys, math, zbar

# Create processor
proc = zbar.Processor()

# Processor config
proc.parse_config('enable')

# Camera config
x_pixel = 640 # 1280
y_pixel = 480 # 720

# QR-Code config (cm)
QR_length = 30
QR_area = QR_length**2

# Device address
device ='/dev/video0'
proc.init(device)

# Setup handler
def data_handler(proc, image, closure):
    for symbol in image.symbols:
        print symbol.data
        current_data = symbol.data
        location = symbol.location
        print get_square_area(location)

# Returns area of square (ineffective?)
def get_square_area(location):
    tot = 0.0
    for i in range(0,4):
        if i == 3:
            j = 0
        else:
            j = i + 1
        tot += location[i][0]*location[j][1]-location[i][1]*location[j][0]
    return abs(0.5*tot)
    
# Adjusts and returns location data from upsidedown
def adjust_location_y(location):
    for i in range(0,4):
        location[i][1] = abs(location[i][1]-y_pixel)
    return location

# Returns the coordinates of the center point in pixel coordinates __UNTESTED__CODE__
def get_square_center(location):
    centerdx = round((location[2][0]-location[0][0])/2, 0)
    centerdy = round((location[2][1]-location[0][1])/2, 0)
    return (location[0][0]+centerdx, location[0][1]+centerdy)

# Returns the difference in x and y pixels from center of camera __UNTESTED__CODE__
def get_center_diff(center_point):
    return (center_point[0]-x_pixel/2, center_point[1]-y_pixel/2)
    
# Returns the angular difference between left & right side __UNTESTED__CODE__
def get_top_angle_deviation(location):
    dx = location[3][0]-location[0][0]
    dy = location[3][1]-location[0][1]
    return math.atan(dx/dy)

def get_bottom_angle_deviation(location):
    dx = location[2][0]-location[1][0]
    dy = location[2][1]-location[1][1]
    return math.atan(dx/dy)

# Returns the distance to the QR-code __UNFINISHED__CODE__
#def get_distance(location):
#    center_point = get_square_center(location)
#    dy_right = location[3][1]-location[2][1]
#    dy_left = location[0][1]-location[1][1]
#    if (dy_right/dy_left) > 1:
#        d = QR_length*((dy_right/dy_left)-1)/2    # If dy1/dy2 -> Right Side, else left side
#    else:
#        d = QR_length*(1-(dy_right/dy_left)/2

# Returns the orientation to cube in degrees
# def get_orientation(bajs):

# from area to distance
# do measurements from camera at measured distances min/max dist?
# from location to current orientation regarding to the front
# TEST PHRASE

proc.set_data_handler(data_handler)

# Preview window
if len(sys.argv) > 1:
    if sys.argv[1]:
        proc.visible = True

# Initiate scanning
proc.active = True
# Alt.
#timeout = 10 # seconds til timeout
#if proc.process_one(timeout):
#   for symbol in proc.results: etc...
#       current_read = symbol.data
#       location = symbol.location
#else:
#   current_read = 0

try:
    proc.user_wait(30)
except (zbar.WindowClosed, KeyboardInterrupt), e:
    sys.exit()

