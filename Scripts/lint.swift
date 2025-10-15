#!/usr/bin/swift

import Foundation

struct LintTool {
    static func main() {
        let args = Array(CommandLine.arguments.dropFirst())
        let command = args.first ?? "lint"
        
        switch command {
        case "format":
            formatSwiftFiles()
        case "lint":
            lintSwiftFiles()
        case "check":
            checkFormatting()
        case "help", "--help", "-h":
            printHelp()
        default:
            print("Unknown command: \(command)")
            printHelp()
            exit(1)
        }
    }
    
    static func formatSwiftFiles() {
        print("🎨 Formatting Swift files...")
        let swiftFiles = findSwiftFiles()
        
        guard !swiftFiles.isEmpty else {
            print("No Swift files found.")
            return
        }
        
        // Use swift-format for formatting
        for file in swiftFiles {
            let result = runCommand("swift-format", ["--in-place", file])
            if result != 0 {
                print("❌ Failed to format \(file)")
                exit(result)
            }
        }
        
        print("✅ Formatted \(swiftFiles.count) Swift files")
    }
    
    static func lintSwiftFiles() {
        print("🔍 Linting Swift files...")
        
        // Run SwiftLint first (comprehensive linting)
        print("Running SwiftLint...")
        let swiftlintResult = runCommand("swiftlint", [])
        
        // Run swift-format lint (formatting check)
        print("Checking formatting with swift-format...")
        let swiftFiles = findSwiftFiles()
        var formatIssues = 0
        
        for file in swiftFiles {
            let result = runCommand("swift-format", ["lint", file], captureOutput: true)
            if result != 0 {
                formatIssues += 1
            }
        }
        
        if swiftlintResult == 0 && formatIssues == 0 {
            print("✅ All Swift files pass linting")
        } else {
            print("❌ Linting issues found")
            if formatIssues > 0 {
                print("💡 Run './Scripts/lint.swift format' to fix formatting issues")
            }
            exit(1)
        }
    }
    
    static func checkFormatting() {
        print("✅ Checking Swift formatting...")
        let swiftFiles = findSwiftFiles()
        var issues = 0
        
        for file in swiftFiles {
            let result = runCommand("swift-format", ["lint", file], captureOutput: true)
            if result != 0 {
                issues += 1
                print("❌ \(file) has formatting issues")
            }
        }
        
        if issues == 0 {
            print("✅ All Swift files are properly formatted")
        } else {
            print("❌ \(issues) files have formatting issues")
            print("💡 Run './Scripts/lint.swift format' to fix")
            exit(1)
        }
    }
    
    static func findSwiftFiles() -> [String] {
        let paths = [
            "esp32-c6-swift-client/main",
            "Tools/Sources"
        ]
        
        var swiftFiles: [String] = []
        
        for path in paths {
            let result = runCommandOutput("find", [path, "-name", "*.swift", "-type", "f"], captureOutput: true)
            if result.exitCode == 0 {
                let files = result.output.components(separatedBy: .newlines)
                    .filter { !$0.isEmpty }
                    .filter { !$0.contains(".build/") }
                swiftFiles.append(contentsOf: files)
            }
        }
        
        return swiftFiles
    }
    
    @discardableResult
    static func runCommand(_ command: String, _ args: [String], captureOutput: Bool = false) -> Int32 {
        let process = Process()
        process.executableURL = URL(fileURLWithPath: "/usr/bin/env")
        process.arguments = [command] + args
        
        if captureOutput {
            process.standardOutput = Pipe()
            process.standardError = Pipe()
        }
        
        do {
            try process.run()
            process.waitUntilExit()
            return process.terminationStatus
        } catch {
            print("❌ Failed to run \(command): \(error)")
            return -1
        }
    }
    
    static func runCommandOutput(_ command: String, _ args: [String], captureOutput: Bool = false) -> (output: String, exitCode: Int32) {
        let process = Process()
        process.executableURL = URL(fileURLWithPath: "/usr/bin/env")
        process.arguments = [command] + args
        
        let pipe = Pipe()
        process.standardOutput = pipe
        process.standardError = pipe
        
        do {
            try process.run()
            process.waitUntilExit()
            
            let data = pipe.fileHandleForReading.readDataToEndOfFile()
            let output = String(data: data, encoding: .utf8) ?? ""
            
            return (output, process.terminationStatus)
        } catch {
            return ("", -1)
        }
    }
    
    static func printHelp() {
        print("""
ESP32-C6 Swift Workshop Linting Tool

USAGE:
    ./Scripts/lint.swift [COMMAND]

COMMANDS:
    format      Auto-format all Swift files using swift-format
    lint        Run comprehensive linting (SwiftLint + swift-format)
    check       Check formatting without making changes (CI-friendly)
    help        Show this help

EXAMPLES:
    ./Scripts/lint.swift format     # Format all Swift files
    ./Scripts/lint.swift lint       # Run all linting checks
    ./Scripts/lint.swift check      # Check formatting for CI

TOOLS USED:
    - SwiftLint: Comprehensive Swift style and convention checking
    - swift-format: Apple's official Swift formatting tool

CONFIGURATION:
    - SwiftLint: .swiftlint.yml (embedded-specific rules)
    - swift-format: .swift-format (2-space indentation)
""")
    }
}

LintTool.main()