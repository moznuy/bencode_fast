from bencode_fast import decode
import pytest


def test_strings():
    with pytest.raises(ValueError, match='Missing ":" after'):
        decode(b"0")
    with pytest.raises(ValueError, match='Missing ":" after'):
        decode(b"10")

    assert "" == decode(b"0:")
    assert "a" == decode(b"1:a")
    assert "abc" == decode(b"3:abc")

    with pytest.raises(ValueError, match="Missing required number of bytes"):
        assert "abc" == decode(b"4:abc")

    with pytest.raises(ValueError, match="Missing required number of bytes"):
        assert "abc" == decode(b"1234567890123456789:abc")
    with pytest.raises(ValueError, match="String is too long"):
        assert "abc" == decode(b"12345678901234567890:abc")

    # sys.gettotalrefcount()
