import requests
import re
import json

# URLs for sending GET requests
horizons_lookup_URL = 'https://ssd.jpl.nasa.gov/api/horizons_lookup.api'
horizons_API_URL = 'https://ssd.jpl.nasa.gov/api/horizons.api'

# All objects that information is needed for
objects = [
    'Sun',
    'Mercury',
    'Venus',
    'Earth',
    'Mars',
    'Jupiter',
    'Saturn',
    'Uranus',
    'Neptune',
    'Pluto',
    'Moon',
    'Europa'
]

# Mappings of obj to its ID for the horizons API request, dict populated by the lookup table step
object_ids = { }

# Final dict to be converted into JSON
json_dict = {
    'count': 0,
    'objects': [

    ]
}

# Iterate through the objects and get the desired objects corresponding ID, place into object_ids
for obj in objects:
    lookup_params = {
        'sstr': obj,
        'group': 'mb'  # Change pln to mb when more than just planets get involved
    }

    request = requests.get(horizons_lookup_URL, params=lookup_params)

    if request.status_code == 200 and int(request.json()['count']) > 0:
        results = request.json()['result']
        for result in results:
            print(result)
            if result['name'].lower() == obj.lower():
                object_ids[obj] = result['spkid']
    else:
        raise ValueError("Object, " + obj + " not found")


print('\n\n')
mass_list = []
radius_list = []
pv_list = []
# Search all object ids in the Horizons API for object data
for entry in object_ids:
    print("QUEUING: " + entry)

    id = object_ids[entry]

    params = {
        'COMMAND': id,
        'EPHEM_TYPE': 'VECTORS',
        'CENTER': '500@0',
        'VEC_TABLE': '2',
        'STEP_SIZE': '1d',
        'START_TIME': '2020-01-01',
        'STOP_TIME': '2020-01-02',
    }

    request = requests.get(horizons_API_URL, params=params)

    if request.status_code == 200:
        result = request.json()['result']
        print(result)

        # Mass[\s|,|x]+10\^[0-9]+\s*\(?kg\)?\s*=[\s|~]+[0-9]+\.?[0-9]+
        mass_str = re.search(r"Mass[\s|,|x]+10\^([0-9]+)\s*\(?kg\)?\s*=[\s|~]+([0-9]+\.?[0-9]+)", result)
        mass = float(str(mass_str.group(2) + 'e' + str(mass_str.group(1))))
        # mass_list.append(entry + ": " + str(mass.group(2)) + "e" + str(mass.group(1)) + "kg")

        radius_str = re.search(r"Vol. [m|M]ean [r|R]adius[\s|,]+\(?km\)?\s+=\s+([0-9]+\.?[0-9]+)", result)
        radius = float(str(radius_str.group(1)))
        # radius_list.append(entry + ": " + str(radius.group(1)) + "km")

        pv = re.search(r"\${2}SOE\n.+\n\s*X\s*=\s*(\-?[0-9]+\.?[0-9]+E[\+|\-][0-9]+)\s*Y\s*=\s*(\-?[0-9]+\.?[0-9]+E[\+|\-][0-9]+)\s*Z\s*=\s*(\-?[0-9]+\.?[0-9]+E[\+|\-][0-9]+)\n\s*VX\s*=\s*(\-?[0-9]+\.?[0-9]+E[\+|\-][0-9]+)\s*VY\s*=\s*(\-?[0-9]+\.?[0-9]+E[\+|\-][0-9]+)\s*VZ\s*=\s*(\-?[0-9]+\.?[0-9]+E[\+|\-][0-9]+)", result)
        X = float(pv.group(1))
        Y = float(pv.group(2))
        Z = float(pv.group(3))
        VX = float(pv.group(4))
        VY = float(pv.group(5))
        VZ = float(pv.group(6))
        # pv_list.append(entry + ': [X ' + pv.group(1) + ', Y ' + pv.group(2) + ', Z ' + pv.group(3) + ', VX ' + pv.group(4) + ', VY ' + pv.group(5) + ', VZ ' + pv.group(6) + ']')

        # Write the values to the JSON dict
        object_dict = {
            'name': entry,
            'mass': mass,
            'radius': radius,
            'X': X,
            'Y': Y,
            'Z': Z,
            'VX': VX,
            'VY': VY,
            'VZ': VZ,
        }
        json_dict['count'] += 1
        json_dict['objects'].append(object_dict)

    else:
        print("REQUEST ERROR: " + request.status_code)


out_file_name = '../planetData/objects.json'
with open(out_file_name, 'w') as outfile:
    json.dump(json_dict, outfile, indent=4)