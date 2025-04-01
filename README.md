## JUCE
Audio applications and plugins created with JUCE

### Getting Started (macOS)
1. Download the VST or application of your choice.
2. Install the latest version of Xcode.
3. Clone the JUCE framework using:  
   `git clone https://github.com/juce-framework/JUCE.git`
4. Navigate to the `/extras/Projucer/Builds/MacOSX` directory and double-click the `Projucer.xcodeproj` file.
5. Click **File** then **New Project**. Under **Plugin**, select **Basic**, and change the project name to match the VST or application you downloaded earlier.
6. Click **Create Project**.
7. Replace the files in the `Source` directory of your newly created project with the files from the `Source` folder of the VST or application you downloaded earlier.
8. Change your scheme to **All**, then click **Play** to compile.
