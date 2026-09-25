import unreal
from pathlib import Path
import math, wave, struct
assets=unreal.AssetToolsHelpers.get_asset_tools()
edit=unreal.MaterialEditingLibrary
mat=unreal.load_asset('/Game/Materials/M_Toy')
if not mat:
    mat=assets.create_asset('M_Toy','/Game/Materials',unreal.Material,unreal.MaterialFactoryNew())
edit.delete_all_material_expressions(mat)
color=edit.create_material_expression(mat,unreal.MaterialExpressionVectorParameter,-400,0)
color.set_editor_property('parameter_name','Color')
color.set_editor_property('default_value',unreal.LinearColor(.6,.3,.1,1))
edit.connect_material_property(color,'',unreal.MaterialProperty.MP_BASE_COLOR)
rough=edit.create_material_expression(mat,unreal.MaterialExpressionConstant,-300,150)
rough.set_editor_property('r',.36)
edit.connect_material_property(rough,'',unreal.MaterialProperty.MP_ROUGHNESS)
ambient=edit.create_material_expression(mat,unreal.MaterialExpressionMultiply,-150,250)
ambient.set_editor_property('const_b',.28)
edit.connect_material_expressions(color,'',ambient,'A')
edit.connect_material_property(ambient,'',unreal.MaterialProperty.MP_EMISSIVE_COLOR)
edit.recompile_material(mat)
unreal.EditorAssetLibrary.save_asset('/Game/Materials/M_Toy')
folder=Path(__file__).parent/'Audio'
folder.mkdir(exist_ok=True)
for path in folder.glob('*.wav'):
    task=unreal.AssetImportTask()
    task.set_editor_property('filename',str(path))
    task.set_editor_property('destination_path','/Game/Audio')
    task.set_editor_property('automated',True);task.set_editor_property('replace_existing',True);task.set_editor_property('save',True)
    assets.import_asset_tasks([task])
    sound=unreal.load_asset('/Game/Audio/'+path.stem)
    if sound:
        sound.set_editor_property('looping',path.stem in ('Music','Ambience'))
        unreal.EditorAssetLibrary.save_asset('/Game/Audio/'+path.stem)
level=unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
if not unreal.EditorAssetLibrary.does_asset_exist('/Game/Maps/PuppyPark'):
    level.new_level('/Game/Maps/PuppyPark');level.save_current_level()
unreal.log('PUP_CONTENT_OK')

