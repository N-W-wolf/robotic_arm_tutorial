# black_task

This folder stores task-competition-specific MuJoCo scene assets
(`scene_terrain.xml` plus field and box assets).

The robotic arm is not duplicated here: `scene_terrain.xml` includes the
shared `robotic_arm.xml` model from the `robotic_description` package through
the `__ARM_MODEL_PATH__` placeholder, which `mujoco_scene_viewer.py` replaces
with the installed model path at load time.
