INCLUDEPATH +=$$PWD/interface
INCLUDEPATH +=$$PWD/src

HEADERS += \
    $$PWD/interface/SKP_Silk_SDK_API.h \
    $$PWD/interface/SKP_Silk_control.h \
    $$PWD/interface/SKP_Silk_errors.h \
    $$PWD/interface/SKP_Silk_typedef.h \
    $$PWD/src/SKP_Silk_AsmHelper.h \
    $$PWD/src/SKP_Silk_AsmPreproc.h \
    $$PWD/src/SKP_Silk_Inlines.h \
    $$PWD/src/SKP_Silk_PLC.h \
    $$PWD/src/SKP_Silk_SigProc_FIX.h \
    $$PWD/src/SKP_Silk_common_pitch_est_defines.h \
    $$PWD/src/SKP_Silk_define.h \
    $$PWD/src/SKP_Silk_macros.h \
    $$PWD/src/SKP_Silk_macros_arm.h \
    $$PWD/src/SKP_Silk_main.h \
    $$PWD/src/SKP_Silk_main_FIX.h \
    $$PWD/src/SKP_Silk_pitch_est_defines.h \
    $$PWD/src/SKP_Silk_resampler_private.h \
    $$PWD/src/SKP_Silk_resampler_rom.h \
    $$PWD/src/SKP_Silk_resampler_structs.h \
    $$PWD/src/SKP_Silk_setup_complexity.h \
    $$PWD/src/SKP_Silk_structs.h \
    $$PWD/src/SKP_Silk_structs_FIX.h \
    $$PWD/src/SKP_Silk_tables.h \
    $$PWD/src/SKP_Silk_tables_NLSF_CB0_10.h \
    $$PWD/src/SKP_Silk_tables_NLSF_CB0_16.h \
    $$PWD/src/SKP_Silk_tables_NLSF_CB1_10.h \
    $$PWD/src/SKP_Silk_tables_NLSF_CB1_16.h \
    $$PWD/src/SKP_Silk_tuning_parameters.h

SOURCES += \
    $$PWD/src/SKP_Silk_A2NLSF.c \
    $$PWD/src/SKP_Silk_CNG.c \
    $$PWD/src/SKP_Silk_HP_variable_cutoff_FIX.c \
    $$PWD/src/SKP_Silk_LBRR_reset.c \
    $$PWD/src/SKP_Silk_LPC_inv_pred_gain.c \
    $$PWD/src/SKP_Silk_LPC_synthesis_filter.c \
    $$PWD/src/SKP_Silk_LPC_synthesis_order16.c \
    $$PWD/src/SKP_Silk_LP_variable_cutoff.c \
    $$PWD/src/SKP_Silk_LSF_cos_table.c \
    $$PWD/src/SKP_Silk_LTP_analysis_filter_FIX.c \
    $$PWD/src/SKP_Silk_LTP_scale_ctrl_FIX.c \
    $$PWD/src/SKP_Silk_MA.c \
    $$PWD/src/SKP_Silk_NLSF2A.c \
    $$PWD/src/SKP_Silk_NLSF2A_stable.c \
    $$PWD/src/SKP_Silk_NLSF_MSVQ_decode.c \
    $$PWD/src/SKP_Silk_NLSF_MSVQ_encode_FIX.c \
    $$PWD/src/SKP_Silk_NLSF_VQ_rate_distortion_FIX.c \
    $$PWD/src/SKP_Silk_NLSF_VQ_sum_error_FIX.c \
    $$PWD/src/SKP_Silk_NLSF_VQ_weights_laroia.c \
    $$PWD/src/SKP_Silk_NLSF_stabilize.c \
    $$PWD/src/SKP_Silk_NSQ.c \
    $$PWD/src/SKP_Silk_NSQ_del_dec.c \
    $$PWD/src/SKP_Silk_PLC.c \
    $$PWD/src/SKP_Silk_VAD.c \
    $$PWD/src/SKP_Silk_VQ_nearest_neighbor_FIX.c \
    $$PWD/src/SKP_Silk_ana_filt_bank_1.c \
    $$PWD/src/SKP_Silk_apply_sine_window.c \
    $$PWD/src/SKP_Silk_array_maxabs.c \
    $$PWD/src/SKP_Silk_autocorr.c \
    $$PWD/src/SKP_Silk_biquad.c \
    $$PWD/src/SKP_Silk_biquad_alt.c \
    $$PWD/src/SKP_Silk_burg_modified.c \
    $$PWD/src/SKP_Silk_bwexpander.c \
    $$PWD/src/SKP_Silk_bwexpander_32.c \
    $$PWD/src/SKP_Silk_code_signs.c \
    $$PWD/src/SKP_Silk_control_audio_bandwidth.c \
    $$PWD/src/SKP_Silk_control_codec_FIX.c \
    $$PWD/src/SKP_Silk_corrMatrix_FIX.c \
    $$PWD/src/SKP_Silk_create_init_destroy.c \
    $$PWD/src/SKP_Silk_dec_API.c \
    $$PWD/src/SKP_Silk_decode_core.c \
    $$PWD/src/SKP_Silk_decode_frame.c \
    $$PWD/src/SKP_Silk_decode_parameters.c \
    $$PWD/src/SKP_Silk_decode_pitch.c \
    $$PWD/src/SKP_Silk_decode_pulses.c \
    $$PWD/src/SKP_Silk_decoder_set_fs.c \
    $$PWD/src/SKP_Silk_detect_SWB_input.c \
    $$PWD/src/SKP_Silk_div_oabi.c \
    $$PWD/src/SKP_Silk_enc_API.c \
    $$PWD/src/SKP_Silk_encode_frame_FIX.c \
    $$PWD/src/SKP_Silk_encode_parameters.c \
    $$PWD/src/SKP_Silk_encode_pulses.c \
    $$PWD/src/SKP_Silk_find_LPC_FIX.c \
    $$PWD/src/SKP_Silk_find_LTP_FIX.c \
    $$PWD/src/SKP_Silk_find_pitch_lags_FIX.c \
    $$PWD/src/SKP_Silk_find_pred_coefs_FIX.c \
    $$PWD/src/SKP_Silk_gain_quant.c \
    $$PWD/src/SKP_Silk_init_encoder_FIX.c \
    $$PWD/src/SKP_Silk_inner_prod_aligned.c \
    $$PWD/src/SKP_Silk_interpolate.c \
    $$PWD/src/SKP_Silk_k2a.c \
    $$PWD/src/SKP_Silk_k2a_Q16.c \
    $$PWD/src/SKP_Silk_lin2log.c \
    $$PWD/src/SKP_Silk_log2lin.c \
    $$PWD/src/SKP_Silk_noise_shape_analysis_FIX.c \
    $$PWD/src/SKP_Silk_pitch_analysis_core.c \
    $$PWD/src/SKP_Silk_pitch_est_tables.c \
    $$PWD/src/SKP_Silk_prefilter_FIX.c \
    $$PWD/src/SKP_Silk_process_NLSFs_FIX.c \
    $$PWD/src/SKP_Silk_process_gains_FIX.c \
    $$PWD/src/SKP_Silk_quant_LTP_gains_FIX.c \
    $$PWD/src/SKP_Silk_range_coder.c \
    $$PWD/src/SKP_Silk_regularize_correlations_FIX.c \
    $$PWD/src/SKP_Silk_resampler.c \
    $$PWD/src/SKP_Silk_resampler_down2.c \
    $$PWD/src/SKP_Silk_resampler_down2_3.c \
    $$PWD/src/SKP_Silk_resampler_down3.c \
    $$PWD/src/SKP_Silk_resampler_private_AR2.c \
    $$PWD/src/SKP_Silk_resampler_private_ARMA4.c \
    $$PWD/src/SKP_Silk_resampler_private_IIR_FIR.c \
    $$PWD/src/SKP_Silk_resampler_private_copy.c \
    $$PWD/src/SKP_Silk_resampler_private_down4.c \
    $$PWD/src/SKP_Silk_resampler_private_down_FIR.c \
    $$PWD/src/SKP_Silk_resampler_private_up2_HQ.c \
    $$PWD/src/SKP_Silk_resampler_private_up4.c \
    $$PWD/src/SKP_Silk_resampler_rom.c \
    $$PWD/src/SKP_Silk_resampler_up2.c \
    $$PWD/src/SKP_Silk_residual_energy16_FIX.c \
    $$PWD/src/SKP_Silk_residual_energy_FIX.c \
    $$PWD/src/SKP_Silk_scale_copy_vector16.c \
    $$PWD/src/SKP_Silk_scale_vector.c \
    $$PWD/src/SKP_Silk_schur.c \
    $$PWD/src/SKP_Silk_schur64.c \
    $$PWD/src/SKP_Silk_shell_coder.c \
    $$PWD/src/SKP_Silk_sigm_Q15.c \
    $$PWD/src/SKP_Silk_solve_LS_FIX.c \
    $$PWD/src/SKP_Silk_sort.c \
    $$PWD/src/SKP_Silk_sum_sqr_shift.c \
    $$PWD/src/SKP_Silk_tables_LTP.c \
    $$PWD/src/SKP_Silk_tables_NLSF_CB0_10.c \
    $$PWD/src/SKP_Silk_tables_NLSF_CB0_16.c \
    $$PWD/src/SKP_Silk_tables_NLSF_CB1_10.c \
    $$PWD/src/SKP_Silk_tables_NLSF_CB1_16.c \
    $$PWD/src/SKP_Silk_tables_gain.c \
    $$PWD/src/SKP_Silk_tables_other.c \
    $$PWD/src/SKP_Silk_tables_pitch_lag.c \
    $$PWD/src/SKP_Silk_tables_pulses_per_block.c \
    $$PWD/src/SKP_Silk_tables_sign.c \
    $$PWD/src/SKP_Silk_tables_type_offset.c \
    $$PWD/src/SKP_Silk_warped_autocorrelation_FIX.c
