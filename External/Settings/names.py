idsToName = {
	"00056375": "Huzzah1",
	"0005659D": "Huzzah2",
	"000AC171": "Alfred",
	"000AC172": "Bob",
	"00D00BD5": "Carl",
	"000AC175": "Darlene",
	"000AC170": "Elaine",
	"000AC173": "Farnsworth"
}

nameToIds = {
	"Huzzah1": "00056375",
	"Huzzah2": "0005659D",
	"Alfred": "000AC171",
	"Bob": "000AC172",
	"Carl": "00D00BD5",
	"Darlene": "000AC175",
	"Elaine": "000AC170",
	"Farnsworth": "000AC173"
}

letterToIds = {
	"H": "00056375",
	"H": "0005659D",
	"A": "000AC171",
	"B": "000AC172",
	"C": "00D00BD5",
	"D": "000AC175",
	"E": "000AC170",
	"F": "000AC173"
}


def checkName(name):
    try:
        return nameToIds[name]
    except KeyError:
        return name

def checkId(ids):
	try:
		return idsToName[ids]
	except KeyError:
		return ids

def convertFromLetter(name):
	try:
		return letterToIds[name]
	except KeyError:
		return name
