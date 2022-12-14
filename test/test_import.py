from bencode_fast import decode
import pytest


def test_strings():
    assert "" == decode(b"0:")
    assert "a" == decode(b"1:a")
    assert "abc" == decode(b"3:abc")

    with pytest.raises(ValueError, match='Missing ":" after'):
        decode(b"0")
    with pytest.raises(ValueError, match='Missing ":" after'):
        decode(b"10")

    # with pytest.raises(ValueError, match="String length can't be negative"):
    #     assert "" == decode(b"-5:abcde")

    with pytest.raises(ValueError, match="Missing required number of bytes"):
        assert "abc" == decode(b"4:abc")

    with pytest.raises(ValueError, match="Missing required number of bytes"):
        assert "abc" == decode(b"1234567890123456789:abc")
    with pytest.raises(ValueError, match="Integer is too large"):
        assert "abc" == decode(b"12345678901234567890:abc")


def test_integers():
    assert 5 == decode(b"i5e")
    assert 15 == decode(b"i15e")
    assert -1234 == decode(b"i-1234e")

    # with pytest.raises(ValueError, match='Missing "i" before'):
    #     decode(b"a")
    # with pytest.raises(ValueError, match='Missing "i" before'):
    #     decode(b"10")

    assert 1234567890123456789 == decode(b"i1234567890123456789e")
    with pytest.raises(ValueError, match="Integer is too large"):
        assert 12345678901234567890 == decode(b"i12345678901234567890e")


def test_list():
    assert [] == decode(b"le")
    assert ["spam", "eggs"] == decode(b"l4:spam4:eggse")

    assert [["spam", "eggs"], ["cat", "dog"]] == decode(b"ll4:spam4:eggsel3:cat3:dogee")
    assert [[], [[]]] == decode(b"llelleee")

    with pytest.raises(ValueError, match='Missing "e" after list'):
        assert [] == decode(b"l4:spam")


def test_not_one_value():
    with pytest.raises(ValueError, match="Supported only 1 value in the top level"):
        assert [] == decode(b"lelele")


def test_zero_byte():
    with pytest.raises(ValueError, match=r"Bytes must not contain '\\0' character"):
        assert [] == decode(b"l\0e")


def test_recursion_limit():
    with pytest.raises(RecursionError):
        decode(b"l" * 200000)


def test_dict():
    assert {} == decode(b"de")
    assert {"cow": "moo", "spam": "eggs"} == decode(b"d3:cow3:moo4:spam4:eggse")

    assert {"spam": ["a", "b"]} == decode(b"d4:spaml1:a1:bee")
    assert {"a": {}, "b": {"c": {}}} == decode(b"d1:ade1:bd1:cdeee")

    with pytest.raises(ValueError, match='Missing "e" after dict'):
        assert {} == decode(b"d4:spami3e")
