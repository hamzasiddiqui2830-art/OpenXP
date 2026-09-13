#!/usr/bin/env python3
"""
Analyze MASM assembly files for calling convention issues.
Checks for inconsistent symbol decoration and macro usage.
"""

import re
import os
from pathlib import Path
from collections import defaultdict

# Paths to analyze
ASM_FILES = [
    "ntoskrnl/ke/i386/clockint.asm",
    "ntoskrnl/ke/i386/cpu.asm",
    "ntoskrnl/ke/i386/ctxswap.asm",
    "ntoskrnl/ke/i386/callout.asm",
]

INCLUDE_FILES = [
    "sdk/sdk/inc/callconv.inc",
    "ntoskrnl/ke/i386/kimacro.inc",
]

BASE_DIR = Path("/workspace")

# Patterns
DECORATED_SYMBOL_PATTERN = re.compile(r'_[A-Za-z_][A-Za-z0-9_]*@\d+')
DOUBLE_DECORATED_PATTERN = re.compile(r'__?[A-Za-z_][A-Za-z0-9_]*@\d+@\d+')
UNDECORATED_SYMBOL_PATTERN = re.compile(r'\b([A-Z][a-zA-Z0-9_]*)\b')
FSTCALL_PATTERN = re.compile(r'fstCall\s+(\S+)')
STDCALL_PATTERN = re.compile(r'stdCall\s+(\S+)')
EXTRNP_PATTERN = re.compile(r'EXTRNP\s+(\S+)\s*:\s*(\S+)')
EXTERN_PATTERN = re.compile(r'EXTERN\s+(\S+)\s*:\s*(\S+)')

def analyze_file(filepath):
    """Analyze a single assembly file."""
    results = {
        'file': str(filepath),
        'double_decorated': [],
        'extrnp_declarations': [],
        'fstcall_calls': [],
        'stdcall_calls': [],
        'potential_issues': [],
    }
    
    if not filepath.exists():
        results['potential_issues'].append(f"File not found: {filepath}")
        return results
    
    with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
        lines = f.readlines()
    
    for line_num, line in enumerate(lines, 1):
        # Check for double-decorated symbols
        if DOUBLE_DECORATED_PATTERN.search(line):
            results['double_decorated'].append({
                'line': line_num,
                'content': line.strip(),
                'match': DOUBLE_DECORATED_PATTERN.search(line).group()
            })
        
        # Check for EXTRNP declarations
        extrnp_match = EXTRNP_PATTERN.search(line)
        if extrnp_match:
            results['extrnp_declarations'].append({
                'line': line_num,
                'symbol': extrnp_match.group(1),
                'type': extrnp_match.group(2)
            })
        
        # Check for fstCall calls
        fstcall_match = FSTCALL_PATTERN.search(line)
        if fstcall_match:
            symbol = fstcall_match.group(1)
            # Remove any trailing comma or arguments
            symbol = symbol.split(',')[0].split('(')[0]
            results['fstcall_calls'].append({
                'line': line_num,
                'symbol': symbol,
                'is_decorated': '@' in symbol
            })
        
        # Check for stdCall calls
        stdcall_match = STDCALL_PATTERN.search(line)
        if stdcall_match:
            symbol = stdcall_match.group(1)
            symbol = symbol.split(',')[0].split('(')[0]
            results['stdcall_calls'].append({
                'line': line_num,
                'symbol': symbol,
                'is_decorated': '@' in symbol
            })
    
    return results

def analyze_include_file(filepath):
    """Analyze macro definition files."""
    results = {
        'file': str(filepath),
        'macro_definitions': {},
        'potential_issues': [],
    }
    
    if not filepath.exists():
        results['potential_issues'].append(f"File not found: {filepath}")
        return results
    
    with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()
    
    # Find macro definitions
    macro_pattern = re.compile(r'(\w+)\s+MACRO\s+(.*?)(?=\n\s*\w+\s+MACRO|\Z)', re.DOTALL)
    for match in macro_pattern.finditer(content):
        macro_name = match.group(1)
        macro_body = match.group(2)
        results['macro_definitions'][macro_name] = macro_body
    
    # Specifically look for fstCall, stdCallCall, stdCall
    for macro_name in ['fstCall', 'stdCallCall', 'stdCall', 'fstENDP', 'stdENDP']:
        if macro_name in results['macro_definitions']:
            body = results['macro_definitions'][macro_name]
            # Check for decoration logic
            if '@' in body or '&@&' in body:
                results['potential_issues'].append(
                    f"Macro {macro_name} contains decoration logic that may cause issues"
                )
    
    return results

def main():
    print("=" * 80)
    print("MASM Assembly Analysis Report")
    print("=" * 80)
    
    all_results = []
    
    # Analyze ASM files
    print("\n### Assembly Files Analysis\n")
    for asm_file in ASM_FILES:
        filepath = BASE_DIR / asm_file
        print(f"Analyzing: {asm_file}")
        results = analyze_file(filepath)
        all_results.append(results)
        
        if results['double_decorated']:
            print(f"  ⚠️  Found {len(results['double_decorated'])} double-decorated symbols:")
            for item in results['double_decorated'][:5]:  # Show first 5
                print(f"     Line {item['line']}: {item['match']}")
            if len(results['double_decorated']) > 5:
                print(f"     ... and {len(results['double_decorated']) - 5} more")
        
        if results['fstcall_calls']:
            decorated_calls = [c for c in results['fstcall_calls'] if c['is_decorated']]
            if decorated_calls:
                print(f"  ⚠️  Found {len(decorated_calls)} fstCall with decorated symbols:")
                for item in decorated_calls[:5]:
                    print(f"     Line {item['line']}: {item['symbol']}")
        
        if results['stdcall_calls']:
            decorated_calls = [c for c in results['stdcall_calls'] if c['is_decorated']]
            if decorated_calls:
                print(f"  ⚠️  Found {len(decorated_calls)} stdCall with decorated symbols:")
                for item in decorated_calls[:5]:
                    print(f"     Line {item['line']}: {item['symbol']}")
        
        if not results['double_decorated'] and not results['fstcall_calls'] and not results['stdcall_calls']:
            print(f"  ✓ No obvious issues found")
        print()
    
    # Analyze include files
    print("\n### Include/Macro Files Analysis\n")
    for inc_file in INCLUDE_FILES:
        filepath = BASE_DIR / inc_file
        print(f"Analyzing: {inc_file}")
        results = analyze_include_file(filepath)
        all_results.append(results)
        
        if 'fstCall' in results['macro_definitions']:
            print(f"  Found fstCall macro definition")
            # Show relevant parts
            body = results['macro_definitions']['fstCall']
            if '@' in body or '&@&' in body:
                print(f"  ⚠️  Macro contains decoration operators")
                # Extract lines with decoration
                for line in body.split('\n'):
                    if '@' in line or '&@&' in line:
                        print(f"     {line.strip()}")
        
        if results['potential_issues']:
            for issue in results['potential_issues']:
                print(f"  ⚠️  {issue}")
        print()
    
    # Summary
    print("\n### Summary\n")
    total_double_decorated = sum(len(r.get('double_decorated', [])) for r in all_results)
    total_fstcall_decorated = sum(
        len([c for c in r.get('fstcall_calls', []) if c.get('is_decorated')])
        for r in all_results
    )
    total_stdcall_decorated = sum(
        len([c for c in r.get('stdcall_calls', []) if c.get('is_decorated')])
        for r in all_results
    )
    
    print(f"Total double-decorated symbols: {total_double_decorated}")
    print(f"Total fstCall with decorated symbols: {total_fstcall_decorated}")
    print(f"Total stdCall with decorated symbols: {total_stdcall_decorated}")
    
    if total_double_decorated > 0 or total_fstcall_decorated > 0 or total_stdcall_decorated > 0:
        print("\n❌ ISSUES FOUND - Manual fix required")
        print("\nRecommendation:")
        print("1. Ensure all fstCall/stdCall invocations use UNDECORATED function names")
        print("2. Let the macros handle symbol decoration automatically")
        print("3. Check callconv.inc for correct decoration logic")
    else:
        print("\n✓ No obvious decoration issues found")
    
    # Save detailed report
    import json
    report_file = BASE_DIR / "asm_analysis_report.json"
    with open(report_file, 'w') as f:
        json.dump(all_results, f, indent=2)
    print(f"\nDetailed report saved to: {report_file}")

if __name__ == "__main__":
    main()
