# NTOS-old to NTOSKRNL Porting Plan - TODO

## Summary
- **ntos-old**: 1338 files (full code with complete implementation)
- **ntoskrnl**: 490 files (stripped down for public release, but has new additions/bug fixes)

## Key Findings

### Files ONLY in ntoskrnl (new additions/bug fixes to preserve):
1. VERIFIER/ directory (entire driver verifier framework)
2. inc/ntdlltrc.h
3. init/initos.c
4. io/PNPMGR/ directory (new PnP manager)
5. ke/gateobj.c, ke/hifreqlk.c, ke/thkemul.c (new kernel object implementations)
6. makefile (updated build system)
7. rtl/error.h, rtl/rtlnthdr.c
8. wmi/ directory enhancements (provider.c, secure.c, smbios.c, traceapi.c, tracelog.c, etc.)

### Files ONLY in ntos-old (missing from ntoskrnl - need to port):
Major directories completely missing:
- arb/ (Arbiter library)
- kd64/ (Kernel debugger 64-bit)
- po/ (Power Manager)
- vdm/ (Virtual DOS Machine)
- ntsym/ (NT Symbols)
- nls/ (National Language Support)

Missing source files in existing directories:
- cache/: prefboot.c, prefetch.c, prefparm.c, preftchp.h (prefetch functionality)
- config/: cmdat3.c, hwprofil.c, cmplock.h (registry enhancements)
- dbgk/: udbgk.c (user-mode debugging)
- ex/: dbgctrl.c, exhotp.c, exinfo.c, hdlsterm.c, intrloc2.c, memprint.c, regtest.c, spintrac.c, sysenv.c, systime.c, tex.c, tlock.c, tprofile.c, xipdisp.c
- fsrtl/: tmcb.c
- fstub/: efi.h, ex.c, ex.h, halfnc.c, translate.c
- inc/: Many header files (arbiter.h, efi*.h, halalpha.h, heap.h, pci.h, tape.h, xip.h, etc.)
- init/: Animation/boot video files, init.c, port.c, ntkrnlpa.c, ntkrpamp.c, ntoskrnl.rc
- io/: iovutil.c, iovputil.h, netboot.c, sessnirp.c, trackirp.c, dumpctl.c, ioperf.c, ioverifier.c, triage.c, pnpmgr/
- ke/: Aligntrk.c, services.tab, tests/
- lpc/: lpclog.c, uclient.c, ulpc.h, userver.c
- mm/: checkpfn.c, checkpte.c, compress.c, crashdmp.c, debugsup.c, dynmem.c, mirror.c, mmpatch.c, nolowmem.c, pfsup.c, specpool.c, triage.c, verifier.c
- ob/: obperf.c, obvutil.c, tob.c, uob.c
- ps/: Full process support files
- rtl/: bootstatus.c, checksum.c, compress.c, handle.c, heap*.c (extensive heap implementation), lookasid.c, lznt1.c, mrcf.c, prodtype.c, random.c, range.c, registry.c, remlock.c, secmem.c, slistfunc.c, threads.c, timer.c, version.c, wait.c, worker.c, xencode.c, xdecode.c
- se/: Extensive security files (auditing, tokens, access control, TSE/RDP support)
- wmi/: diags.c, kdexts/, sample/

### Files that DIFFER (need careful merge):
439 files differ between the two trees. The differences include:
1. Copyright/license headers (ntos-old has Microsoft copyright, ntoskrnl has GPL)
2. Type changes (KSPIN_LOCK -> ALIGNED_SPINLOCK in ntoskrnl)
3. Debug code removal (CCDBG sections removed in ntoskrnl)
4. Bug fixes and improvements in ntoskrnl
5. Code stripped for public release in ntoskrnl

## TODO Tasks

### Phase 1: Copy Missing Directories from ntos-old to ntoskrnl
1. [ ] Copy arb/ directory (complete)
2. [ ] Copy kd64/ directory (complete)
3. [ ] Copy po/ directory (complete)
4. [ ] Copy vdm/ directory (complete)
5. [ ] Copy ntsym/ directory (complete)
6. [ ] Copy nls/ directory (complete)

### Phase 2: Copy Missing Source Files
7. [ ] Copy cache prefetch files (prefboot.c, prefetch.c, prefparm.c, preftchp.h)
8. [ ] Copy config missing files (cmdat3.c, hwprofil.c, cmplock.h)
9. [ ] Copy dbgk/udbgk.c
10. [ ] Copy ex/ missing files (dbgctrl.c, exhotp.c, exinfo.c, hdlsterm.c, intrloc2.c, memprint.c, regtest.c, spintrac.c, sysenv.c, systime.c, tex.c, tlock.c, tprofile.c, xipdisp.c)
11. [ ] Copy fsrtl/tmcb.c
12. [ ] Copy fstub/ missing files (efi.h, ex.c, ex.h, halfnc.c, translate.c)
13. [ ] Copy inc/ missing headers
14. [ ] Copy init/ missing files (init.c, port.c, animation files, resource files)
15. [ ] Copy io/ missing files (iovutil.c, iovputil.h, netboot.c, sessnirp.c, trackirp.c, dumpctl.*, ioperf.c, ioverifier.c, triage.c, pnpmgr/)
16. [ ] Copy ke/ missing files (aligntrk.c, services.tab, tests/)
17. [ ] Copy lpc/ missing files (lpclog.c, uclient.c, ulpc.h, userver.c)
18. [ ] Copy mm/ missing files (checkpfn.c, checkpte.c, compress.c, crashdmp.c, debugsup.c, dynmem.c, mirror.c, mmpatch.c, nolowmem.c, pfsup.c, specpool.c, triage.c, verifier.c)
19. [ ] Copy ob/ missing files (obperf.c, obvutil.c, tob.c, uob.c)
20. [ ] Copy ps/ missing files
21. [ ] Copy rtl/ missing files (heap implementation, compression, etc.)
22. [ ] Copy se/ missing files (security auditing, tokens, TSE)
23. [ ] Copy wmi/ missing files (diags.c, kdexts/, sample/)

### Phase 3: Merge Differences in Common Files
For each of the 439 differing files:
- Preserve ntoskrnl's license headers (GPL)
- Preserve ntoskrnl's type changes (ALIGNED_SPINLOCK, etc.)
- Port bug fixes from ntoskrnl to the newly copied files where applicable
- Add back stripped functionality from ntos-old where it doesn't conflict

### Phase 4: Build System Updates
24. [ ] Update makefiles to include newly added files
25. [ ] Verify directory structure matches expected layout
26. [ ] Update sources.inc files for new files

### Phase 5: Verification
27. [ ] Verify all files are present
28. [ ] Check for any remaining missing symbols
29. [ ] Test build if possible

## Notes
- The ntoskrnl tree has valuable additions (VERIFIER, PNPMGR, WMI enhancements) that must be preserved
- License headers need to remain as GPL (ntoskrnl style)
- Type definitions may differ (KSPIN_LOCK vs ALIGNED_SPINLOCK)
- Debug code (CCDBG, DBG) was stripped from ntoskrnl - decide whether to include in final
