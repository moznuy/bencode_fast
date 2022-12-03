import sys

import bencodemodule
import pytest


def test_strings():
    with pytest.raises(ValueError, match="missing after"):
        bencodemodule.decode(b"0")
    with pytest.raises(ValueError, match="missing after"):
        bencodemodule.decode(b"10")

    assert "" == bencodemodule.decode(b"0:")
    assert "a" == bencodemodule.decode(b"1:a")
    assert "abc" == bencodemodule.decode(b"3:abc")

    with pytest.raises(ValueError, match="missing required number of bytes"):
        assert "abc" == bencodemodule.decode(b"4:abc")

    sys.gettotalrefcount()
