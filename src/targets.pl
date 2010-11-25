my @obj_files;

## compiler flags ##
@defines = qw( );
if (defined($debug) && $debug) {
  push (@defines, "DEBUG");
}
## end compiler flags ##

## libraries to link ##
my @libs;
unless ($disable_wine) {
  push (@libs, 'ole32','msvcrt','winmm','gdi32','ddraw','jpeg','dinput','dplayx','dsound','winmm','shell32');
}
## end libraries to link ##

## library paths ##
if (defined($dxsdk_path)) {
  push (@lib_dirs, "$dxsdk_path/lib");
}
## end library paths ##

## include paths ##
@includes = qw( ../include );

if (!$disable_wine && defined($wine_prefix)) {
  push (@includes, "$wine_prefix/include/wine/windows",
                   "$wine_prefix/include/wine/msvcrt");
}

if (defined($dxsdk_path)) {
  push (@includes, "$dxsdk_path/include");
}
## include paths ##

@targets = qw(
EnglishToChinese.cpp
am.cpp
oai_act.cpp
oai_act2.cpp
oai_attk.cpp
oai_buil.cpp
oai_cap2.cpp
oai_capt.cpp
oai_defe.cpp
oai_dipl.cpp
oai_eco.cpp
oai_gran.cpp
oai_info.cpp
oai_main.cpp
oai_mili.cpp
oai_quer.cpp
oai_seek.cpp
oai_spy.cpp
oai_talk.cpp
oai_unit.cpp
oanline.cpp
oattack.cpp
oaudio.cpp
ob_flame.cpp
ob_homin.cpp
ob_prog.cpp
ob_proj.cpp
obaseoba.cpp
obaseobj.cpp
obattle.cpp
obitmap.cpp
obitmapw.cpp
oblob.cpp
oblob2.cpp
oblob2w.cpp
obox.cpp
obullet.cpp
obulleta.cpp
obutt3d.cpp
obuttcus.cpp
obutton.cpp
oc_eas10.cpp
oc_eas11.cpp
oc_eas12.cpp
oc_eas13.cpp
oc_eas14.cpp
oc_eas15.cpp
oc_eas16.cpp
oc_eas17.cpp
oc_eas18.cpp
oc_eas19.cpp
oc_eas20.cpp
oc_eas21.cpp
oc_eas22.cpp
oc_eas23.cpp
oc_eas24.cpp
oc_eas25.cpp
oc_eas26.cpp
oc_eas27.cpp
oc_eas28.cpp
oc_east.cpp
oc_east1.cpp
oc_east2.cpp
oc_east3.cpp
oc_east4.cpp
oc_east5.cpp
oc_east6.cpp
oc_east7.cpp
oc_east8.cpp
oc_east9.cpp
oc_easte.cpp
oc_eastp.cpp
oc_pla1.cpp
oc_pla2.cpp
oc_pla3.cpp
oc_pla4.cpp
oc_pla5.cpp
oc_pla6.cpp
oc_pla7.cpp
oc_plc1.cpp
oc_plc2.cpp
oc_plc3.cpp
oc_plc4.cpp
oc_pld1.cpp
oc_pld2.cpp
oc_pld3.cpp
oc_pld4.cpp
oc_pld5.cpp
oc_plot.cpp
ocampgn.cpp
ocampgn2.cpp
ocampgn3.cpp
ocampgn4.cpp
ocnation.cpp
ocoltbl.cpp
oconfig.cpp
ocrc_sto.cpp
odate.cpp
odb.cpp
odir.cpp
odplay.cpp
odynarr.cpp
odynarrb.cpp
oeffect.cpp
oend_con.cpp
oerrctrl.cpp
oerror.cpp
oevent.cpp
oexpmask.cpp
of_alch.cpp
of_alcha.cpp
of_alchi.cpp
of_anim.cpp
of_anima.cpp
of_animi.cpp
of_base.cpp
of_basea.cpp
of_basei.cpp
of_basew.cpp
of_camp.cpp
of_campa.cpp
of_campb.cpp
of_campi.cpp
of_camps.cpp
of_campu.cpp
of_fact.cpp
of_facta.cpp
of_facti.cpp
of_fort.cpp
of_forta.cpp
of_forti.cpp
of_incu.cpp
of_incua.cpp
of_incui.cpp
of_inn.cpp
of_inna.cpp
of_inni.cpp
of_lair.cpp
of_laira.cpp
of_lairi.cpp
of_lara2.cpp
of_lish.cpp
of_lishi.cpp
of_magi.cpp
of_magia.cpp
of_magii.cpp
of_mark.cpp
of_marka.cpp
of_marki.cpp
of_mfori.cpp
of_mfork.cpp
of_mfort.cpp
of_mine.cpp
of_minea.cpp
of_minei.cpp
of_monsi.cpp
of_mtrn.cpp
of_mtrna.cpp
of_mtrni.cpp
of_off.cpp
of_off2.cpp
of_off2i.cpp
of_offi.cpp
of_rese.cpp
of_resea.cpp
of_resei.cpp
of_spec.cpp
of_speca.cpp
of_speci.cpp
of_spy.cpp
of_spya.cpp
of_spyi.cpp
of_traia.cpp
of_traii.cpp
of_train.cpp
of_war.cpp
of_wara.cpp
of_wari.cpp
of_work.cpp
of_worka.cpp
of_worki.cpp
ofile.cpp
ofiletxt.cpp
ofirm.cpp
ofirma.cpp
ofirmai.cpp
ofirmat.cpp
ofirmdie.cpp
ofirmdrw.cpp
ofirmif.cpp
ofirmlnk.cpp
ofirmres.cpp
oflame.cpp
oflc.cpp
ofont.cpp
ofontchi.cpp
oformat.cpp
ogamcamp.cpp
ogamcred.cpp
ogamdemo.cpp
ogame.cpp
ogamecon.cpp
ogamemp.cpp
ogamend.cpp
ogameset.cpp
ogamhall.cpp
ogammain.cpp
ogamsce2.cpp
ogamscen.cpp
ogamsing.cpp
ogenhill.cpp
ogenmap.cpp
ogenmap2.cpp
oget.cpp
ogeta.cpp
ogfile.cpp
ogfile2.cpp
ogfile3.cpp
ogfilea.cpp
ogodres.cpp
ohelp.cpp
oherores.cpp
ohillres.cpp
ohsetres.cpp
oimgres.cpp
oimmplay.cpp
oinfo.cpp
oingmenu.cpp
oisoarea.cpp
oitem.cpp
oitemres.cpp
ojpeg.cpp
olightn.cpp
olightn2.cpp
olinear.cpp
olocate.cpp
olocbmp.cpp
olog.cpp
olonglog.cpp
olzw.cpp
omagic.cpp
omatrix.cpp
omatrix2.cpp
omem.cpp
omisc.cpp
omlink.cpp
omodeid.cpp
omonsres.cpp
omouse.cpp
omousecr.cpp
omousefr.cpp
omousege.cpp
omouseit.cpp
omouseng.cpp
omousesp.cpp
omp_crc.cpp
omusic.cpp
on_human.cpp
on_mons.cpp
onationa.cpp
onationb.cpp
onews.cpp
onews2.cpp
onewseng.cpp
onewsfre.cpp
onewsger.cpp
onewsspa.cpp
ooptmenu.cpp
opfind.cpp
oplace.cpp
oplant.cpp
oplasma.cpp
opower.cpp
opower2.cpp
opreuse.cpp
oprofil2.cpp
oprofile.cpp
or_ai.cpp
or_eco.cpp
or_mil.cpp
or_nat.cpp
or_news.cpp
or_rank.cpp
or_spy.cpp
or_tech.cpp
or_town.cpp
or_trade.cpp
oraceres.cpp
orain1.cpp
orain2.cpp
orain3.cpp
orawres.cpp
orebel.cpp
oregion.cpp
oregions.cpp
oremote.cpp
oremote2.cpp
oremotem.cpp
oremoteq.cpp
ores.cpp
oresdb.cpp
orespal.cpp
orestxt.cpp
oresx.cpp
orle.cpp
orock.cpp
orockres.cpp
oscroll.cpp
ose.cpp
osedit_e.cpp
osedit_f.cpp
osedit_h.cpp
osedit_m.cpp
osedit_o.cpp
osedit_p.cpp
osedit_r.cpp
osedit_v.cpp
oseditor.cpp
oseres.cpp
oserial.cpp
osfrmres.cpp
osite.cpp
ositedrw.cpp
oskill.cpp
oslidcus.cpp
oslider.cpp
osnow1.cpp
osnow2.cpp
osnowg.cpp
osnowres.cpp
osoldier.cpp
ospin_s.cpp
ospinner.cpp
osprite.cpp
osprite2.cpp
ospritea.cpp
osprtres.cpp
ospy.cpp
ospy2.cpp
ospya.cpp
ostate.cpp
ostr.cpp
osysdet.cpp
osysfile.cpp
osysproc.cpp
osysseed.cpp
osysview.cpp
ot_basic.cpp
ot_camp.cpp
ot_endc.cpp
ot_firm.cpp
ot_gmenu.cpp
ot_item.cpp
ot_news.cpp
ot_reps.cpp
ot_sedit.cpp
ot_talk.cpp
ot_tutor.cpp
ot_unit.cpp
otalkeng.cpp
otalkfre.cpp
otalkger.cpp
otalkmsg.cpp
otalkres.cpp
otalkspa.cpp
oteam.cpp
otechres.cpp
oterrain.cpp
otips.cpp
otornado.cpp
otown.cpp
otowna.cpp
otownai.cpp
otownat.cpp
otownbld.cpp
otowndef.cpp
otowndrw.cpp
otownif.cpp
otownind.cpp
otownlnk.cpp
otownpop.cpp
otownres.cpp
otownwal.cpp
otrainee.cpp
otransl.cpp
otutor.cpp
otutor2.cpp
otutor3.cpp
otwalres.cpp
otxtres.cpp
ou_cara.cpp
ou_caraa.cpp
ou_carai.cpp
ou_caras.cpp
ou_carat.cpp
ou_cart.cpp
ou_god.cpp
ou_god2.cpp
ou_godi.cpp
ou_mons.cpp
ou_wagon.cpp
oun_act.cpp
oun_ai.cpp
oun_ai2.cpp
oun_atk.cpp
oun_atk2.cpp
oun_drw.cpp
oun_grp.cpp
oun_hero.cpp
oun_if.cpp
oun_ind.cpp
oun_init.cpp
oun_info.cpp
oun_item.cpp
oun_mode.cpp
oun_ord.cpp
oun_proc.cpp
oun_spab.cpp
ounadraw.cpp
ounita.cpp
ounitb.cpp
ounitres.cpp
ovbrowi2.cpp
ovbrowif.cpp
ovbrowse.cpp
ovga2.cpp
ovgabuf2.cpp
ovideo.cpp
ovolume.cpp
ovqueue.cpp
ow_fire.cpp
ow_plant.cpp
ow_rock.cpp
ow_sound.cpp
ow_trv.cpp
ow_wall.cpp
owallres.cpp
owarpt.cpp
owaypnt.cpp
oweather.cpp
oworld.cpp
oworld_m.cpp
oworld_z.cpp
);
#unless ($disable_wine) {
#  push (@targets, 'ico.rc');
#}

push (@obj_files, build_targets(\@targets, \@includes, \@defines));
push (@obj_files, include_targets('asm/targets.pl'));

# code that touches dd_obj is handled a bit differently
@targets = qw(
osysdisp.cpp
osysinit.cpp
ovga.cpp
ovgabuf.cpp
ovgalock.cpp
);
push (@defines, '__DDRAW_INCLUDED__');
push (@obj_files, build_targets(\@targets, \@includes, \@defines));

my $client_exe_name = '7kaa';
unless ($disable_wine) {
  $client_exe_name .= '.exe';
}
link_exe ($client_exe_name,
          \@obj_files,
          \@libs,
          \@lib_dirs);


1;
