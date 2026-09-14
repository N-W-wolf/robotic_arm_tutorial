# MDH parameters

This teaching package uses the modified Denavit-Hartenberg convention:

```text
A_i = RotX(alpha_(i-1)) * TransX(a_(i-1))
      * RotZ(theta_i) * TransZ(d_i)
```

The four URDF joint axes are intentionally set to `+Z`. Therefore the
positive URDF joint positions are also the positive MDH joint variables.

| joint | a_(i-1) [m] | alpha_(i-1) [rad] | d_i [m] | theta_i |
|---|---:|---:|---:|---|
| joint1 | 0.00 | 0.00 | 0.12 | q1 |
| joint2 | 0.00 | +pi/2 | 0.00 | q2 |
| joint3 | 0.18 | 0.00 | 0.00 | q3 |
| joint4 | 0.18 | 0.00 | 0.00 | q4 |

The corresponding product is:

```text
T_04 = Rz(q1) * Tz(0.12)
       * Rx(pi/2) * Rz(q2)
       * Tx(0.18) * Rz(q3)
       * Tx(0.18) * Rz(q4)
```

These values describe the kinematic convention used by this teaching
package. The original SolidWorks-exported package remains unchanged and
keeps its original joint-axis signs.
