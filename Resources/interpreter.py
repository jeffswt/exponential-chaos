
import os
import re
import _csv as csv

fin = open('entity.csv')
cw = csv.reader(fin)
lst = list()
for row in cw:
    lst.append(row)
fin.close()
first_line = lst[0]

def mproc(a):
    if a == 'TRUE': return 'true'
    if a == 'FALSE': return 'false'
    return a

for nm in range(1, len(lst)):
    idx = dict()
    for i in range(0, len(lst[nm])):
        idx[first_line[i]] = mproc(lst[nm][i])
    nstr = """
{
    "Properties": {
        "Name": "%s",
        "Type": "%s",
        "Description": "%s",
        "ShowInCreative": %s
    },
    "Physics": {
        "PhysicsEnabled": %s,
        "CollisionEnabled": %s,
        "LengthX": %s,
        "LengthY": %s,
        "Mass": %s,
        "FrictionFactorTop": %s,
        "FrictionFactorBottom": %s,
        "FrictionFactorLeft": %s,
        "FrictionFactorRight": %s,
        "ElasticFactorTop": %s,
        "ElasticFactorBottom": %s,
        "ElasticFactorLeft": %s,
        "ElasticFactorRight": %s,
        "BlastResistance": %s
    },
    "Graphics": {
        "RenderEnabled": %s,
        "LengthX": %s,
        "LengthY": %s,
        "AnimationInterval": %s,
        "TextureOnHand": "%s",
        "TextureList": [
            "%s"
        ]
    }
}""" % (idx['Name'], idx['Type'], idx['Description'], idx['ShowInCreative'], idx['PhysicsEnabled'], idx['CollisionEnabled'], idx['pLengthX'], idx['pLengthY'], idx['Mass'], idx['FrictionFactorTop'], idx['FrictionFactorBottom'], idx['FrictionFactorLeft'], idx['FrictionFactorRight'], idx['ElasticFactorTop'], idx['ElasticFactorBottom'], idx['ElasticFactorLeft'], idx['ElasticFactorRight'], idx['BlastResistance'], idx['RenderEnabled'], idx['gLengthX'], idx['gLengthY'], idx['AnimationInterval'], idx['TextureOnHand'], idx['Texture'])
    ofn = './output/' + idx['Texture'].replace('.png', '.json')
    dirs = re.sub('/[^/]*\\.json', '', ofn)
    print('Writing to file: %s' % ofn)
    try:
        os.makedirs(dirs)
    except FileExistsError:
        pass
    fout = open(ofn, 'w', encoding='utf-8')
    fout.write(nstr)
    fout.close()

print('done')
