import sys,re

def parseEntry(entry):
    matches = re.search(
        "Name: (.*?)\n" +
        "Original Size: (.*?)\n" +
        "Compressed Size: (.*?)\n" +
        "Compression Rate: (.*?)\n" +
        "Elapsed time \(C\): (.*?)\n" + 
        "Elapsed time \(D\): (.*?)\n"
    , entry, re.MULTILINE | re.I | re.DOTALL)
    fileName=matches[1]
    oSize = float(matches[2])
    cSize = float(matches[3])
    cRate = matches[4]
    elapsedTimeC = matches[5]
    elapsedTimeD = matches[6]

    if(oSize != 0):
        cRate = (1 - cSize/oSize) * 100
    else:
        cRate = 0

    elapsedTimeC_split = elapsedTimeC.split(":")
    elapsedTimeD_split = elapsedTimeD.split(":")
    time_c = float(elapsedTimeC_split[0]) * 60 + float(elapsedTimeC_split[1])
    time_d = float(elapsedTimeD_split[0]) * 60 + float(elapsedTimeD_split[1])

    if time_c == 0:
        compressionSpeed = 0
    else:
        compressionSpeed = float(oSize)/float(time_c)

    if time_d == 0:
        decompressionSpeed = 0
    else:
        decompressionSpeed = float(oSize)/float(time_d)

    compressionSpeed /= 1024
    decompressionSpeed /= 1024

    oSize /= 1024
    cSize /= 1024

    if 0:
        print(
            (
                "File: {}\n" +
                "oSize: {}\n" +
                "cSize: {}\n" + 
                "cRate: {}\n" + 
                "elapsedTimeC: {}\n" + 
                "elapsedD: {}\n" + 
                "compressionSpeed: {:.1f}\n" +
                "decompressionSpeed: {:.1f}\n\n"
            ).format(fileName, oSize, cSize, cRate, time_c, time_d, compressionSpeed, decompressionSpeed)
        )

        print(elapsedTimeC_split)
    
    print(
        (
            "|{}|{:.1f}|{:.1f}|{:.1f}%|{:.1f}|{:.1f}|{:.1f}|{:.1f}|"
        ).format(fileName, oSize, cSize, cRate, time_c, time_d, compressionSpeed, decompressionSpeed)
    )


file = sys.argv[1]

with open(file, "r") as myfile:
    data=myfile.read()

#print(data)
result=re.finditer(r"=====(.*?)===", data, re.MULTILINE | re.I | re.DOTALL)
print("| Nome file | O. Size [kB] | C. Size [kB] | Rate | C [s] | D [s] | C [kB/s] | D [kB/s] |")
print("| --------- | ------------- | ---------- | ---- | ----- | ----- | -------- | -------- |")
for i in result:
    parseEntry(i[1])
