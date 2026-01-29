#include "Editor.h"

static juce::WebBrowserComponent::Options getCommonWebViewOptions()
{
    return juce::WebBrowserComponent::Options().withNativeIntegrationEnabled()
                                               .withBackend (juce::WebBrowserComponent::Options::Backend::webview2)
                                               .withWinWebView2Options (juce::WebBrowserComponent::Options::WinWebView2().withUserDataFolder (juce::File::getSpecialLocation (juce::File::tempDirectory)));
}

#if defined(WEB_VIEW_URL)
 static juce::WebBrowserComponent::Options getStaticWebViewOptions()
 {
     return getCommonWebViewOptions().withInitialisationData ("isLocalServer", true);
 }
#else
 static const char* getMimeForExtension (const juce::String& extension) 
 {
     static const std::unordered_map<juce::String, const char*> mimeMap = {
         { { "htm" },    "text/html" },
         { { "html" },   "text/html" },
         { { "txt" },    "text/plain" },
         { { "jpg" },    "image/jpeg" },
         { { "jpeg" },   "image/jpeg" },
         { { "svg" },    "image/svg+xml" },
         { { "ico" },    "image/vnd.microsoft.icon" },
         { { "json" },   "application/json" },
         { { "png" },    "image/png" },
         { { "css" },    "text/css" },
         { { "map" },    "application/json" },
         { { "js" },     "text/javascript" },
         { { "woff2" },  "font/woff2" }
     };
 
     if (const auto it = mimeMap.find (extension.toLowerCase()); it != mimeMap.end())
         return it->second;
 
     jassertfalse;
     return "";
 }
 
 static std::vector<std::byte> streamToVector (juce::InputStream& stream)
 {
     std::vector<std::byte> result (stream.getTotalLength());
     stream.setPosition (0);
 
     const juce::int64 bytesRead = stream.read (result.data(), result.size());
     jassert (bytesRead == stream.getTotalLength());
 
     return result;
 }
 
 static std::optional<juce::WebBrowserComponent::Resource> getResource (const juce::String& url) 
 {
     const auto relPath = [&] () -> juce::String
     {
         if (url == "/")
             return "index.html";
         else
             return url.fromFirstOccurrenceOf ("/", false, false);
     }();
 
     juce::MemoryInputStream zipFileStream (BinaryData::dist_zip, BinaryData::dist_zipSize, false);
     juce::ZipFile zip (zipFileStream);
     std::unique_ptr<juce::InputStream> fileStream (zip.createStreamForEntry (zip.getIndexOfFileName (relPath, false)));
 
     if (fileStream)
         return std::make_optional<juce::WebBrowserComponent::Resource> (streamToVector (*fileStream), 
                                                                         getMimeForExtension (relPath.fromLastOccurrenceOf (".", false, false)));
 
     return std::nullopt;
 }
 
 static juce::WebBrowserComponent::Options getStaticWebViewOptions()
 {
     return getCommonWebViewOptions().withResourceProvider ([] (const juce::String& url) { return getResource (url); })
                                     .withInitialisationData ("isLocalServer", false);
 }
#endif

Editor::Editor (Processor& p)
  : AudioProcessorEditor (&p), 
    audioProcessor (p),
    #define X(name) name##Relay (p.parameters.name->paramID), name##Attachment (*p.parameters.name, name##Relay),
    TAPESTOP_PARAMETERS (X)
    #undef X
    webView (getWebViewOptions())
{
    addAndMakeVisible (webView);

   #if defined(WEB_VIEW_URL)
    webView.goToURL (WEB_VIEW_URL);
   #else
    webView.goToURL (juce::WebBrowserComponent::getResourceProviderRoot());
   #endif

    setSize (320, 560);
}

Editor::~Editor()
{
}

void Editor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkslategrey);
}

void Editor::resized()
{
    webView.setBounds (getLocalBounds());
}

juce::WebBrowserComponent::Options Editor::getWebViewOptions()
{
    #define X(name) .withOptionsFrom (name##Relay)
    auto options = getStaticWebViewOptions() TAPESTOP_PARAMETERS (X);
    #undef X

    const auto params = [this]
    {
        const auto obj = new juce::DynamicObject();

        #define X(name) obj->setProperty (#name, audioProcessor.parameters.name->paramID);
        TAPESTOP_PARAMETERS (X)
        #undef X

        return juce::var (obj);
    }();

    const auto choices = [this]
    {
        const auto obj = new juce::DynamicObject();

        #define X(name) \
            if (auto choiceParam = dynamic_cast<juce::AudioParameterChoice*> (audioProcessor.parameters.name.get())) \
                obj->setProperty (audioProcessor.parameters.name->paramID, choiceParam->choices); \
            else \
                obj->setProperty (audioProcessor.parameters.name->paramID, juce::StringArray());
        TAPESTOP_PARAMETERS (X)
        #undef X

        return juce::var (obj);
    }();

    const auto defaultValues = [this]
    {
        const auto obj = new juce::DynamicObject();

        #define X(name) obj->setProperty (audioProcessor.parameters.name->paramID, audioProcessor.parameters.name->getDefaultValue());
        TAPESTOP_PARAMETERS (X)
        #undef X

        return juce::var (obj);
    }();

    return options.withInitialisationData ("params", params)
                  .withInitialisationData ("defaultValues", defaultValues)
                  .withInitialisationData ("choices", choices);
}
