import configparser,re,pathlib
cfg=configparser.ConfigParser();cfg.optionxform=str;cfg.read('config/PostalBorkenMenu.ini')
rows=re.findall(r'\{"[^"\n]+", L"[^"\n]+", L"([^"\n]+)", (\d+),0,0,0,0,0,0,\{',pathlib.Path('source/PostalBorkenMenu_part1.inc').read_text())
assert rows and all(value=='0' for _,value in rows)
assert all(value=='0' for value in cfg['Keys'].values())
assert all(cfg['Keys'].get(name)==value for name,value in rows if name not in ['SkipIntroVideos','SkipStartupLogos'])
src=pathlib.Path('source/PostalBorkenMenu_part3.inc').read_text()
for name,value in [('MenuKey',112),('UpdaterBeta',1),('NoCrosshair',1),('SkipStartupLogos',1),('LevelTimer',1)]:
 assert cfg['Settings'][name]==str(value)
 assert f'L"{name}",{value},g_iniPath' in src
assert cfg['Visual']['WeaponPostProcessing']=='0'
assert 'L"WeaponPostProcessing",0,g_iniPath' in src
assert all(cfg['Visual'][name]=='0' for name in ['NoHud','NoHudWithCrossHair','NoModel','NoMenu'])
print('PASS: all command defaults unbound; personal settings aligned in ASI and release INI; F1 menu preserved')

assert cfg['Args']['WeaponKeysMode']=='OFF'
assert "{L'O',L'F',L'F',0}" in pathlib.Path('source/PostalBorkenMenu_part1.inc').read_text()
