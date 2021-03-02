// Koka generated module: "std/core/types", koka version: 2.0.14
#include "std_core_types.h"

kk_std_core_types__tuple2_ kk_std_core_types__copy_1(kk_std_core_types__tuple2_ _this, kk_std_core_types__optional fst0, kk_std_core_types__optional snd0, kk_context_t* _ctx) { /* forall<a,b> ((a, b), fst : optional<a>, snd : optional<b>) -> (a, b) */ 
  kk_box_t _x2560;
  if (kk_std_core_types__is_Optional(fst0)) {
    kk_box_t _fst_378 = fst0._cons.Optional.value;
    _x2560 = _fst_378; /*399*/
  }
  else {
    kk_box_t _x = _this.fst;
    kk_box_dup(_x);
    _x2560 = _x; /*399*/
  }
  kk_box_t _x2561;
  if (kk_std_core_types__is_Optional(snd0)) {
    kk_box_t _snd_386 = snd0._cons.Optional.value;
    kk_std_core_types__tuple2__drop(_this, _ctx);
    _x2561 = _snd_386; /*400*/
  }
  else {
    kk_box_t _x0 = _this.snd;
    kk_box_dup(_x0);
    kk_std_core_types__tuple2__drop(_this, _ctx);
    _x2561 = _x0; /*400*/
  }
  return kk_std_core_types__new_dash__lp__comma__rp_(_x2560, _x2561, _ctx);
}

kk_std_core_types__tuple3_ kk_std_core_types__copy_2(kk_std_core_types__tuple3_ _this, kk_std_core_types__optional fst0, kk_std_core_types__optional snd0, kk_std_core_types__optional thd0, kk_context_t* _ctx) { /* forall<a,b,c> ((a, b, c), fst : optional<a>, snd : optional<b>, thd : optional<c>) -> (a, b, c) */ 
  kk_box_t _x2562;
  if (kk_std_core_types__is_Optional(fst0)) {
    kk_box_t _fst_570 = fst0._cons.Optional.value;
    _x2562 = _fst_570; /*637*/
  }
  else {
    kk_box_t _x = _this.fst;
    kk_box_dup(_x);
    _x2562 = _x; /*637*/
  }
  kk_box_t _x2563;
  if (kk_std_core_types__is_Optional(snd0)) {
    kk_box_t _snd_611 = snd0._cons.Optional.value;
    _x2563 = _snd_611; /*638*/
  }
  else {
    kk_box_t _x0 = _this.snd;
    kk_box_dup(_x0);
    _x2563 = _x0; /*638*/
  }
  kk_box_t _x2564;
  if (kk_std_core_types__is_Optional(thd0)) {
    kk_box_t _thd_620 = thd0._cons.Optional.value;
    kk_std_core_types__tuple3__drop(_this, _ctx);
    _x2564 = _thd_620; /*639*/
  }
  else {
    kk_box_t _x1 = _this.thd;
    kk_box_dup(_x1);
    kk_std_core_types__tuple3__drop(_this, _ctx);
    _x2564 = _x1; /*639*/
  }
  return kk_std_core_types__new_dash__lp__comma__comma__rp_(_x2562, _x2563, _x2564, _ctx);
}

kk_std_core_types__tuple4_ kk_std_core_types__copy_3(kk_std_core_types__tuple4_ _this, kk_std_core_types__optional fst0, kk_std_core_types__optional snd0, kk_std_core_types__optional thd0, kk_std_core_types__optional field40, kk_context_t* _ctx) { /* forall<a,b,c,d> ((a, b, c, d), fst : optional<a>, snd : optional<b>, thd : optional<c>, field4 : optional<d>) -> (a, b, c, d) */ 
  kk_box_t _x2569;
  if (kk_std_core_types__is_Optional(fst0)) {
    kk_box_t _fst_967 = fst0._cons.Optional.value;
    _x2569 = _fst_967; /*1113*/
  }
  else {
    struct kk_std_core_types__Tuple4_* _con2570 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(_this);
    kk_box_t _x = _con2570->fst;
    kk_box_dup(_x);
    _x2569 = _x; /*1113*/
  }
  kk_box_t _x2571;
  if (kk_std_core_types__is_Optional(snd0)) {
    kk_box_t _snd_1031 = snd0._cons.Optional.value;
    _x2571 = _snd_1031; /*1114*/
  }
  else {
    struct kk_std_core_types__Tuple4_* _con2572 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(_this);
    kk_box_t _x0 = _con2572->snd;
    kk_box_dup(_x0);
    _x2571 = _x0; /*1114*/
  }
  kk_box_t _x2573;
  if (kk_std_core_types__is_Optional(thd0)) {
    kk_box_t _thd_1082 = thd0._cons.Optional.value;
    _x2573 = _thd_1082; /*1115*/
  }
  else {
    struct kk_std_core_types__Tuple4_* _con2574 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(_this);
    kk_box_t _x1 = _con2574->thd;
    kk_box_dup(_x1);
    _x2573 = _x1; /*1115*/
  }
  kk_box_t _x2575;
  if (kk_std_core_types__is_Optional(field40)) {
    kk_box_t _field4_1092 = field40._cons.Optional.value;
    kk_std_core_types__tuple4__dropn(_this, ((int32_t)4), _ctx);
    _x2575 = _field4_1092; /*1116*/
  }
  else {
    struct kk_std_core_types__Tuple4_* _con2576 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(_this);
    kk_box_t _pat03 = _con2576->fst;
    kk_box_t _pat14 = _con2576->snd;
    kk_box_t _pat23 = _con2576->thd;
    kk_box_t _x2 = _con2576->field4;
    if (kk_std_core_types__tuple4__is_unique(_this)) {
      kk_box_drop(_pat23, _ctx);
      kk_box_drop(_pat14, _ctx);
      kk_box_drop(_pat03, _ctx);
      kk_std_core_types__tuple4__free(_this);
    }
    else {
      kk_box_dup(_x2);
      kk_std_core_types__tuple4__decref(_this, _ctx);
    }
    _x2575 = _x2; /*1116*/
  }
  return kk_std_core_types__new_dash__lp__comma__comma__comma__rp_(kk_reuse_null, _x2569, _x2571, _x2573, _x2575, _ctx);
}

kk_std_core_types___lp__comma__comma__comma__comma__rp_ kk_std_core_types__copy_4(kk_std_core_types___lp__comma__comma__comma__comma__rp_ _this, kk_std_core_types__optional fst0, kk_std_core_types__optional snd0, kk_std_core_types__optional thd0, kk_std_core_types__optional field40, kk_std_core_types__optional field50, kk_context_t* _ctx) { /* forall<a,b,c,d,a1> ((a, b, c, d, a1), fst : optional<a>, snd : optional<b>, thd : optional<c>, field4 : optional<d>, field5 : optional<a1>) -> (a, b, c, d, a1) */ 
  kk_box_t _x2582;
  if (kk_std_core_types__is_Optional(fst0)) {
    kk_box_t _fst_1668 = fst0._cons.Optional.value;
    _x2582 = _fst_1668; /*1935*/
  }
  else {
    struct kk_std_core_types__lp__comma__comma__comma__comma__rp_* _con2583 = kk_std_core_types__as_dash__lp__comma__comma__comma__comma__rp_(_this);
    kk_box_t _x = _con2583->fst;
    kk_box_dup(_x);
    _x2582 = _x; /*1935*/
  }
  kk_box_t _x2584;
  if (kk_std_core_types__is_Optional(snd0)) {
    kk_box_t _snd_1760 = snd0._cons.Optional.value;
    _x2584 = _snd_1760; /*1936*/
  }
  else {
    struct kk_std_core_types__lp__comma__comma__comma__comma__rp_* _con2585 = kk_std_core_types__as_dash__lp__comma__comma__comma__comma__rp_(_this);
    kk_box_t _x0 = _con2585->snd;
    kk_box_dup(_x0);
    _x2584 = _x0; /*1936*/
  }
  kk_box_t _x2586;
  if (kk_std_core_types__is_Optional(thd0)) {
    kk_box_t _thd_1838 = thd0._cons.Optional.value;
    _x2586 = _thd_1838; /*1937*/
  }
  else {
    struct kk_std_core_types__lp__comma__comma__comma__comma__rp_* _con2587 = kk_std_core_types__as_dash__lp__comma__comma__comma__comma__rp_(_this);
    kk_box_t _x1 = _con2587->thd;
    kk_box_dup(_x1);
    _x2586 = _x1; /*1937*/
  }
  kk_box_t _x2588;
  if (kk_std_core_types__is_Optional(field40)) {
    kk_box_t _field4_1899 = field40._cons.Optional.value;
    _x2588 = _field4_1899; /*1938*/
  }
  else {
    struct kk_std_core_types__lp__comma__comma__comma__comma__rp_* _con2589 = kk_std_core_types__as_dash__lp__comma__comma__comma__comma__rp_(_this);
    kk_box_t _x2 = _con2589->field4;
    kk_box_dup(_x2);
    _x2588 = _x2; /*1938*/
  }
  kk_box_t _x2590;
  if (kk_std_core_types__is_Optional(field50)) {
    kk_box_t _field5_1910 = field50._cons.Optional.value;
    kk_std_core_types___lp__comma__comma__comma__comma__rp__dropn(_this, ((int32_t)5), _ctx);
    _x2590 = _field5_1910; /*1939*/
  }
  else {
    struct kk_std_core_types__lp__comma__comma__comma__comma__rp_* _con2591 = kk_std_core_types__as_dash__lp__comma__comma__comma__comma__rp_(_this);
    kk_box_t _pat04 = _con2591->fst;
    kk_box_t _pat15 = _con2591->snd;
    kk_box_t _pat24 = _con2591->thd;
    kk_box_t _pat34 = _con2591->field4;
    kk_box_t _x3 = _con2591->field5;
    if (kk_std_core_types___lp__comma__comma__comma__comma__rp__is_unique(_this)) {
      kk_box_drop(_pat34, _ctx);
      kk_box_drop(_pat24, _ctx);
      kk_box_drop(_pat15, _ctx);
      kk_box_drop(_pat04, _ctx);
      kk_std_core_types___lp__comma__comma__comma__comma__rp__free(_this);
    }
    else {
      kk_box_dup(_x3);
      kk_std_core_types___lp__comma__comma__comma__comma__rp__decref(_this, _ctx);
    }
    _x2590 = _x3; /*1939*/
  }
  return kk_std_core_types__new_dash__lp__comma__comma__comma__comma__rp_(kk_reuse_null, _x2582, _x2584, _x2586, _x2588, _x2590, _ctx);
}

kk_std_core_types__hbox kk_std_core_types__copy_6(kk_std_core_types__hbox _this, kk_std_core_types__optional unhbox0, kk_context_t* _ctx) { /* forall<a> (hbox<a>, unhbox : optional<a>) -> hbox<a> */ 
  kk_box_t _x2594;
  if (kk_std_core_types__is_Optional(unhbox0)) {
    kk_box_t _unhbox_2200 = unhbox0._cons.Optional.value;
    kk_std_core_types__hbox_dropn(_this, ((int32_t)1), _ctx);
    _x2594 = _unhbox_2200; /*2209*/
  }
  else {
    struct kk_std_core_types_Hbox* _con2595 = kk_std_core_types__as_Hbox(_this);
    kk_box_t _x = _con2595->unhbox;
    if (kk_std_core_types__hbox_is_unique(_this)) {
      kk_std_core_types__hbox_free(_this);
    }
    else {
      kk_box_dup(_x);
      kk_std_core_types__hbox_decref(_this, _ctx);
    }
    _x2594 = _x; /*2209*/
  }
  return kk_std_core_types__new_Hbox(kk_reuse_null, _x2594, _ctx);
}

kk_reuse_t kk_std_core_types_no_reuse(kk_context_t* _ctx) { /* () -> reuse */ 
  return NULL;
}

// initialization
void kk_std_core_types__init(kk_context_t* _ctx){
  static bool _initialized = false;
  if (_initialized) return;
  _initialized = true;
}
