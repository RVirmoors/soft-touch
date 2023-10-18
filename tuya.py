import tinytuya

# Connect to Device
d = tinytuya.OutletDevice(
    dev_id='bf537aqqarrwx2rp',
    address='192.168.50.34',      # Or set to 'Auto' to auto-discover IP address
    local_key='fh9gs4au9jt7dk3rv74k', 
    version=3.3)

# Get Status
data = d.status() 
print('set_status() result %r' % data)

# # Turn On
# d.turn_on()

# # Turn Off
# d.turn_off()