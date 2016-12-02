from bidict import bidict

#: Our native client's starting position (76, 76).
#: We will use this to offset positioning.
NATIVE_ORIGIN_XY = 76



# TODO Constants on our native client
EDGE_EMPTY = 0
EDGE_CITY = 1
EDGE_FIELD = 2
EDGE_ROAD = 3

# TODO Shields need to be refactored into distinct prey animals.
ATTRIB_NONE = 0
ATTRIB_SHIELD = 1
ATTRIB_MONASTERY = 2

BIDICT_ROT_NATIVE_TGZ = bidict({
    0: 0,
    1: 270,
    2: 180,
    3: 90,
})

#: bidict for converting network representations of tile strings
#: to and from native representations of tile structs.
BIDICT_TILE_STR_ABYTE = bidict({
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

#: dict for converting *truncated* network representation of tile strings
#: (omit trailing feature character) to respective tiger placement,
#: indexed by native meeple placement.
DICT_TILE_MEEPLE_TIGER = dict({
    # "strTile" : (
    #     # Tuple index
    #       0 1 2
    #     11     3
    #     10     4
    #      9     5
    #       8 7 6
    # )
    "JJJJ" : (
        1, 1, 1,
        1, 1, 1,
        1, 1, 1,
        1, 1, 1,
    ),
    "JJTJ" : (
        1, 1, 1,
        1, 1, 1,
        1, 8, 1,
        1, 1, 1,
    ),
    "TTTT" : (
        1, 2, 3,
        3, 6, 9,
        9, 8, 7,
        7, 4, 1,
    ),
    "TJTJ" : (
        1, 2, 3,
        3, 3, 3,
        3, 2, 1,
        1, 1, 1,
    ),
    "TJJT" : (
        1, 2, 3,
        3, 3, 3,
        3, 3, 3,
        3, 2, 1,
    ),
    "TJTT" : (
        1, 2, 3,
        3, 3, 3,
        3, 8, 7,
        7, 4, 1,
    ),
    "LLLL" : (
        1, 1, 1,
        1, 1, 1,
        1, 1, 1,
        1, 1, 1,
    ),
    "JLLL" : (
        1, 1, 1,
        4, 4, 4,
        4, 4, 4,
        4, 4, 4,
    ),
    "LLJJ" : (
        2, 2, 2,
        2, 2, 2,
        1, 1, 1,
        1, 1, 1,
    ),
    "JLJL" : (
        1, 1, 1,
        4, 4, 4,
        7, 7, 7,
        4, 4, 4,
    ),
    "LJLJ" : (
        2, 2, 2,
        1, 1, 1,
        8, 8, 8,
        1, 1, 1,
    ),
    "LJJJ" : (
        2, 2, 2,
        1, 1, 1,
        1, 1, 1,
        1, 1, 1,
    ),
    "JLLJ" : (
        1, 1, 1,
        6, 6, 6,
        8, 8, 8,
        1, 1, 1,
    ),
    "TLJT" : (
        1, 2, 6,
        6, 6, 6,
        3, 3, 3,
        3, 4, 1,
    ),
    "JLTT" : (
        1, 1, 1,
        6, 6, 6,
        1, 8, 7,
        7, 4, 1,
    ),
    "TLTJ" : (
        1, 2, 3,
        6, 6, 6,
        3, 2, 1,
        1, 1, 1,
    ),
    "TLLL" : (
        1, 2, 3,
        4, 4, 4,
        4, 4, 4,
        4, 4, 4,
    ),
    "TLTT" : (
        1, 2, 3,
        6, 6, 6,
        3, 8, 7,
        7, 4, 1,
    ),
    "TLLT" : (
        1, 2, 3,
        6, 6, 6,
        6, 6, 6,
        3, 4, 1,
    ),
    "LJTJ" : (
        2, 2, 2,
        3, 3, 3,
        3, 5, 1, # TODO Confirm that road placement is definitely 5, not 8
        1, 1, 1,
    ),
})

#: dict for converting *truncated* network representation of tile strings
#: (omit trailing feature character) to respective native meeple placement,
#: indexed by network tiger placement.
#: -1 represents impossible intersection placements.
DICT_TILE_TIGER_MEEPLE = dict({
    "JJJJ" : (
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
    ),
    "JJTJ" : (
        0, 0, 0,
        0, 0, 0,
        0, 7, 0,
    ),
    "TTTT" : (
        0, 1, 2,
        10, -1, 4,
        8, 7, 6,
    ),
    "TJTJ" : (
        0, 1, 2,
        0, 1, 2,
        0, 1, 2,
    ),
    "TJJT" : (
        0, 1, 2,
        1, 1, 2,
        2, 2, 2,
    ),
    "TJTT" : (
        0, 1, 2,
        10, -1, 4,
        8, 7, 6,
    ),
    "LLLL" : (
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
    ),
    "JLLL" : (
        0, 0, 0,
        3, 0, 3,
        3, 3, 3,
    ),
    "LLJJ" : (
        6, 0, 0,
        6, 6, 0,
        6, 6, 6,
    ),
    "JLJL" : (
        0, 0, 0,
        4, 4, 4,
        6, 6, 6,
    ),
    "LJLJ" : (
        3, 1, 3,
        3, 3, 3,
        3, 7, 3,
    ),
    "LJJJ" : (
        3, 1, 3,
        3, 3, 3,
        3, 3, 3,
    ),
    "JLLJ" : (
        0, 0, 0,
        0, 0, 4,
        0, 7, 0,
    ),
    "TLJT" : (
        0, 1, 2,
        1, 1, 4,
        2, 2, 2,
    ),
    "JLTT" : (
        0, 0, 0,
        7, 7, 4,
        9, 7, 0,
    ),
    "TLTJ" : (
        0, 1, 2,
        0, 1, 4,
        0, 1, 2,
    ),
    "TLTT" : (
        0, 1, 2,
        10, -1, 4,
        9, 7, 2,
    ),
    "TLLT" : (
        0, 1, 2,
        1, 1, 3, # TODO Confirm centre is considered road.
        2, 3, 3,
    ),
    "LJTJ" : (
        9, 1, 5,
        9, 7, 5,
        9, 7, 5,
    ),
    "TLLL" : (
        0, 1, 2,
        4, 4, 4,
        4, 4, 4,
    ),
})

def translate_tile_network_to_native(strTile):
    """Given the network string, returns our native byte array translation."""
    return bytearray(BIDICT_TILE_STR_ABYTE[strTile])

def translate_tile_native_to_network(aintTile):
    """Given our native byte array, returns the string translation."""
    return BIDICT_TILE_STR_ABYTE.inv[tuple(aintTile)]

def translate_placement_meeple_to_tiger(aintTile, intMeeple):
    """
    Given our native byte array and index to place meeple at,
    returns the respective tiger index
    """
    strTile = BIDICT_TILE_STR_ABYTE.inv[tuple(aintTile)]

    # Special-case tiles featuring a den.
    if strTile.endswith('X') and intMeeple >= 12:
        return 5

    return DICT_TILE_MEEPLE_TIGER[strTile[:-1]][intMeeple]

def translate_placement_tiger_to_meeple(strTile, intTiger):
    """
    Given the network string and index to place tiger at,
    returns our respective meeple index.

    But if an invalid tiger placement, returns -1.

    Contrary to TigerZone rules,
    this function does not enforce minimum zone number placement
    and will return a meeple index as if such a rule did not exist.
    """
    # Special-case tiles featuring a den.
    if strTile.endswith('X') and intTiger == 5:
        return 12

    return DICT_TILE_TIGER_MEEPLE[strTile[:-1]][intTiger-1]

def translate_coord_network_to_native(x, y):
    """Given network coordinates, returns our native coordinates as tuple."""
    return (x+NATIVE_ORIGIN_XY, y+NATIVE_ORIGIN_XY)

def translate_coord_native_to_network(x, y):
    """
    Given our native coordinates,
    returns coordinates normalised for the network as tuple
    """
    return (x-NATIVE_ORIGIN_XY, y-NATIVE_ORIGIN_XY)

def translate_rotation_network_to_native(i):
    """
    Given anti-clockwise degree rotation,
    returns our native clockwise rotation index.
    """
    return BIDICT_ROT_NATIVE_TGZ.inv[i]

def translate_rotation_native_to_network(i):
    """
    Given our native clockwise rotation index,
    returns anti-clockwise degree rotation.
    """
    return BIDICT_ROT_NATIVE_TGZ[i]

def main():
    # TODO Unit tests for all the things:
    # - Cursory sweep to see if all tiles exist in dictionaries.
    # - Den detection
    # - Check all possible cases of "jungle favouritism"
    # - Check all known cases of invalid tiger placement
    # - Check little slivers of land (*literal* corner cases)
    print translate_tile_network_to_native("JJJJ-")
    print translate_tile_native_to_network([2, 2, 2, 2, 2, 2])
    print translate_placement_meeple_to_tiger([2, 2, 2, 2, 2, 2], 12)
    print translate_placement_meeple_to_tiger([2, 2, 2, 2, 2, 2], 5)
    print translate_placement_tiger_to_meeple("JJJJX", 5)
    print translate_placement_tiger_to_meeple("JJJJX", 9)
    print translate_coord_native_to_network(0, 0)
    print translate_coord_network_to_native(0, 0)
    print translate_rotation_native_to_network(1)
    print translate_rotation_network_to_native(90)

main()
