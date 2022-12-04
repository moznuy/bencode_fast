from bencode_fast import decode
import pytest


@pytest.mark.skip
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


def test_integers():
    with pytest.raises(ValueError, match='Missing "i" before'):
        decode(b"a")
    with pytest.raises(ValueError, match='Missing "i" before'):
        decode(b"10")

    assert 5 == decode(b"i5e")
    assert 15 == decode(b"i15e")
    assert -1234 == decode(b"i-1234e")

    assert 1234567890123456789 == decode(b"i1234567890123456789e")
    with pytest.raises(ValueError, match="Integer is too long"):
        assert 12345678901234567890 == decode(b"i12345678901234567890e")
