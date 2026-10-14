"""Read-only evidence check; needs UnityPy, never writes game assets."""
import sys
import struct
from pathlib import Path
import UnityPy

root = Path(sys.argv[1])
env = UnityPy.load(str(root / 'resources.assets'))
objects = {o.path_id: o for o in env.objects}
names = {o.path_id: o.read().m_Name for o in env.objects if o.type.name == 'Sprite'}
manager = UnityPy.load(str(root / 'globalgamemanagers'))
rm = next(o for o in manager.objects if o.type.name == 'ResourceManager')
entries = rm.read_typetree()['m_Container']
entry = next((k, v) for k, v in entries if k == 'postalplayer_dlc')
print('Resource entry:', entry)
print('External file:', rm.assets_file.externals[entry[1]['m_FileID'] - 1].path)
prefab_id = entry[1]['m_PathID']
seen = set()
found = set()

def walk(go_id):
    if go_id in seen:
        return
    seen.add(go_id)
    go = objects[go_id].read_typetree()
    for component in go['m_Component']:
        ptr = component['component']
        if ptr['m_FileID'] or not ptr['m_PathID']:
            continue
        obj = objects[ptr['m_PathID']]
        try:
            tree = obj.read_typetree()
        except ValueError:
            # IL2CPP scripted layouts may be stripped: bounded serialized PPtr
            # matches prove references, not the field's managed runtime type.
            data = obj.get_raw_data()
            for pid, name in names.items():
                if name.startswith('Weapon Icon_') and name.endswith('_Rose'):
                    offset = data.find(struct.pack('<iq', 0, pid))
                    if offset >= 0:
                        found.add(name)
                        print('Serialized component reference:', go['m_Name'], obj.path_id, name, offset)
            continue
        sprite = tree.get('m_Sprite', {})
        if sprite.get('m_FileID') == 0 and sprite.get('m_PathID') in names:
            name = names[sprite['m_PathID']]
            if name.startswith('Weapon Icon_') and name.endswith('_Rose'):
                found.add(name)
        for child in tree.get('m_Children', []):
            if child['m_FileID'] == 0 and child['m_PathID']:
                transform = objects[child['m_PathID']].read_typetree()
                walk(transform['m_GameObject']['m_PathID'])

walk(prefab_id)
print('Prefab game objects traversed:', len(seen))
print('DLC Rose sprite references in prefab descendants:', sorted(found))
