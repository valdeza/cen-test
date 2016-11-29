from bidict import bidict

# TODO Constants on our native client
EDGE_EMPTY = 0
EDGE_CITY = 1
EDGE_FIELD = 2
EDGE_ROAD = 3

# TODO Shields need to be refactored into distinct prey animals.
ATTRIB_NONE = 0
ATTRIB_SHIELD = 1
ATTRIB_MONASTERY = 2

TILE_DICT = bidict({
    "JJJJ-" : (
        EDGE_FIELD, EDGE_FIELD, EDGE_FIELD, EDGE_FIELD, EDGE_FIELD,
        ATTRIB_NONE
    ),
    "JJJJX" : (
        EDGE_FIELD, EDGE_FIELD, EDGE_FIELD, EDGE_FIELD, EDGE_FIELD,
        ATTRIB_MONASTERY
    ),
    "JJTJX" : (
        EDGE_FIELD, EDGE_FIELD, EDGE_ROAD, EDGE_FIELD, EDGE_FIELD,
        ATTRIB_MONASTERY
    ),
    "TTTT-" : (
        EDGE_ROAD, EDGE_ROAD, EDGE_ROAD, EDGE_ROAD, EDGE_ROAD,
        ATTRIB_NONE
    ),
    "TJTJ-" : (
        EDGE_ROAD, EDGE_FIELD, EDGE_ROAD, EDGE_FIELD, EDGE_ROAD,
        ATTRIB_NONE
    ),
    "TJJT-" : (
        EDGE_ROAD, EDGE_FIELD, EDGE_FIELD, EDGE_ROAD, EDGE_ROAD,
        ATTRIB_NONE
    ),
    "TJTT-" : (
        EDGE_ROAD, EDGE_FIELD, EDGE_ROAD, EDGE_ROAD, EDGE_ROAD,
        ATTRIB_NONE
    ),
    "LLLL-" : (
        EDGE_CITY, EDGE_CITY, EDGE_CITY, EDGE_CITY, EDGE_CITY,
        ATTRIB_NONE
    ),
    "JLLL-" : (
        EDGE_FIELD, EDGE_CITY, EDGE_CITY, EDGE_CITY, EDGE_CITY,
        ATTRIB_NONE
    ),
    "LLJJ-" : (
        EDGE_CITY, EDGE_CITY, EDGE_FIELD, EDGE_FIELD, EDGE_CITY,
        ATTRIB_NONE
    ),
    "JLJL-" : (
        EDGE_FIELD, EDGE_CITY, EDGE_FIELD, EDGE_CITY, EDGE_CITY,
        ATTRIB_NONE
    ),
    "LJLJ-" : (
        EDGE_CITY, EDGE_FIELD, EDGE_CITY, EDGE_FIELD, EDGE_FIELD,
        ATTRIB_NONE
    ),
    "LJJJ-" : (
        EDGE_CITY, EDGE_FIELD, EDGE_FIELD, EDGE_FIELD, EDGE_FIELD,
        ATTRIB_NONE
    ),
    "JLLJ-" : (
        EDGE_FIELD, EDGE_CITY, EDGE_CITY, EDGE_FIELD, EDGE_FIELD,
        ATTRIB_NONE
    ),
    "TLJT-" : (
        EDGE_ROAD, EDGE_CITY, EDGE_FIELD, EDGE_ROAD, EDGE_ROAD,
        ATTRIB_NONE
    ),
    "TLJTP" : (
        EDGE_ROAD, EDGE_CITY, EDGE_FIELD, EDGE_ROAD, EDGE_ROAD,
        ATTRIB_SHIELD # Boar
    ),
    "JLTT-" : (
        EDGE_FIELD, EDGE_CITY, EDGE_ROAD, EDGE_ROAD, EDGE_ROAD,
        ATTRIB_NONE
    ),
    "JLTTB" : (
        EDGE_FIELD, EDGE_CITY, EDGE_ROAD, EDGE_ROAD, EDGE_ROAD,
        ATTRIB_SHIELD # Buffalo
    ),
    "TLTJ-" : (
        EDGE_ROAD, EDGE_CITY, EDGE_ROAD, EDGE_FIELD, EDGE_ROAD,
        ATTRIB_NONE
    ),
    "TLTJD" : (
        EDGE_ROAD, EDGE_CITY, EDGE_ROAD, EDGE_FIELD, EDGE_ROAD,
        ATTRIB_SHIELD # Deer
    ),
    "TLLL-" : (
        EDGE_ROAD, EDGE_CITY, EDGE_CITY, EDGE_CITY, EDGE_CITY,
        ATTRIB_NONE
    ),
    "TLTT-" : (
        EDGE_ROAD, EDGE_CITY, EDGE_ROAD, EDGE_ROAD, EDGE_ROAD,
        ATTRIB_NONE
    ),
    "TLTTP" : (
        EDGE_ROAD, EDGE_CITY, EDGE_ROAD, EDGE_ROAD, EDGE_ROAD,
        ATTRIB_SHIELD # Boar
    ),
    "TLLT-" : (
        EDGE_ROAD, EDGE_CITY, EDGE_CITY, EDGE_ROAD, EDGE_CITY,
        ATTRIB_NONE
    ),
    "TLLTB" : (
        EDGE_ROAD, EDGE_CITY, EDGE_CITY, EDGE_ROAD, EDGE_CITY,
        ATTRIB_SHIELD # Buffalo
    ),
    "LJTJ-" : (
        EDGE_CITY, EDGE_FIELD, EDGE_ROAD, EDGE_FIELD, EDGE_ROAD,
        ATTRIB_NONE
    ),
    "LJTJD" : (
        EDGE_CITY, EDGE_FIELD, EDGE_ROAD, EDGE_FIELD, EDGE_ROAD,
        ATTRIB_SHIELD # Deer
    ),
    "TLLLC" : (
        EDGE_ROAD, EDGE_CITY, EDGE_CITY, EDGE_CITY, EDGE_CITY,
        ATTRIB_SHIELD # Crocodile
    )
})

def translate_tile_network_to_native(strTile):
    """Given a string, returns our native byte array translation."""
    return bytearray(TILE_DICT[strTile])

def translate_tile_native_to_network(aintTile):
    """Given our native byte array, returns the string translation."""
    return TILE_DICT.inv[tuple(aintTile)]

def main():
    print translate_tile_network_to_native("JJJJ-")
    print translate_tile_native_to_network([2, 2, 2, 2, 2, 0])

main()
