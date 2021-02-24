idsToName = {
	"00056375": "Huzzah1",
	"0005659D": "Huzzah2",
	"000AC171": "Alfred",
	"000AC172": "Bob",
	"00D00BD5": "Carl",
	"000AC175": "Darlene",
	"000AC170": "Elaine",
	"000AC173": "Farnsworth",
	"00D00BD2": "Gina",
	"00D00BCB": "Hiro",
	"00D00BD1": "Itchy",
	"00D00BD0": "Jerry",
	"00D00BCE": "Karen",
	"00D00BCF": "Lucille",
	"00D00BCD": "Maurice",
	"00D00BCA": "Nancy",	
}

nameToIds = {
	"Huzzah1": "00056375",
	"Huzzah2": "0005659D",
	"Alfred": "000AC171",
	"Bob": "000AC172",
	"Carl": "00D00BD5",
	"Darlene": "000AC175",
	"Elaine": "000AC170",
	"Farnsworth": "000AC173",
	"Gina":"00D00BD2",
	"Hiro":"00D00BCB",
	"Itchy":"00D00BD1",
	"Jerry":"00D00BD0",
	"Karen":"00D00BCE",
	"Lucille":"00D00BCF",
	"Maurice":"00D00BCD",
	"Nancy":"00D00BCA",
}

letterToIds = {
	"Z": "00056375",
	"Z": "0005659D",
	"A": "000AC171",
	"B": "000AC172",
	"C": "00D00BD5",
	"D": "000AC175",
	"E": "000AC170",
	"F": "000AC173",
	"G": "00D00BD2",
	"H": "00D00BCB",
	"I": "00D00BD1",
	"J": "00D00BD0",
	"K": "00D00BCE",
	"L": "00D00BCF",
	"M": "00D00BCD",
	"N": "00D00BCA",	
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
