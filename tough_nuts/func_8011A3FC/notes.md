# func_8011A3FC — vec3 magnitude (fp-reg routing)

`return sqrt(x^2 + y^2 + z^2)` via func_80036090 (sqrt). Math correct
but mul.s ops use different fp regs than base.
