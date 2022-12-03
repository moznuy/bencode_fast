import warnings
from typing import Dict
from typing import List
from typing import Union

try:
    import bencode_fast.bencodemodule as bencodemodule
except ImportError:
    import bencode_fast.slow as bencodemodule

    warnings.warn("Could not import C ext, using slow implementation", RuntimeWarning)


def decode(s: bytes) -> Union[Dict, List, int, str]:
    return bencodemodule.decode(s)


__all__ = [
    "decode",
]
