//
//  BaseProcessor.m
//  MELTS
//
//  Created by Mark Ghiorso on 1/7/13.
//  Copyright (c) 2013 Mark Ghiorso. All rights reserved.
//

#import "BaseProcessor.h"
#import "AppDelegate.h"

#include <pthread.h>

@implementation BaseProcessor

-(id)init {
    if ((self = [super init])) {
        _respondUsingJSON = NO;
        _inputXML = nil;
        _debug = YES;
    }
    return self;
}

-(NSString *)parseAndValidateInputXMLAsString:(NSString *)inputXMLString orAsData:(NSData *)inputXMLData  {
    NSError *error = nil;
    if (inputXMLString)    [self setInputXML:[[NSXMLDocument alloc] initWithXMLString:inputXMLString options:NSXMLNodeOptionsNone error:&error]];
    else if (inputXMLData) [self setInputXML:[[NSXMLDocument alloc] initWithData:inputXMLData        options:NSXMLNodeOptionsNone error:&error]];
    else                   return @"Internal error in creation of XML Document";
    if (error) return [error description];
    
    NSString *xsdPath = [[NSBundle mainBundle] pathForResource:@"MELTSinput" ofType:@"xsd"];
    NSXMLNode *noNamespaceSchemaLocation = [NSXMLNode attributeWithName:@"xsi:noNamespaceSchemaLocation" stringValue:xsdPath];
    NSXMLElement *rootElement = [[self inputXML] rootElement];
    NSMutableArray *rootAttributes = [[rootElement attributes] mutableCopy];
    [rootAttributes replaceObjectAtIndex:1 withObject:noNamespaceSchemaLocation];
    [rootElement setAttributes:rootAttributes];
    if (![[self inputXML] validateAndReturnError:&error]) return @"The input XML is NOT valid";
    if (error) return [error description];
    
    return nil;
}

-(id)renderWithTransport:(BxTransport *)transport {
    if (self.debug) {
        unsigned long long tid;
        pthread_threadid_np(pthread_self(), &tid);
        NSLog(@"<*><*><*>BaseProcessor Class:(renderWithTransport, pid = %d, thread = %llu) Entry...", getpid(), tid);
        NSLog(@"... Query variables:  %@", [[transport queryVars] description]);
        NSLog(@"... Post variables:   %@", [[transport postVars] description]);
        NSLog(@"... Server variables: %@", [[transport serverVars] description]);
        NSLog(@"... Cookies:          %@", [[transport cookies] description]);
        [(AppDelegate *)[self app] lockState];
        NSLog(@"... State:            %@", [[[self app] state] description]);
        [(AppDelegate *)[self app] unloackState];
        if ([transport rawPostData]) NSLog(@"... Raw post data are available");
        else NSLog(@"... Raw post data are not available.");
    }
    [transport setHeader:@"Access-Control-Allow-Origin" value:@"*"];
    [transport setHeader:@"Access-Control-Allow-Methods" value:@"GET,POST,OPTIONS"];
    [transport setHeader:@"Access-Control-Allow-Headers" value:@"Content-Type"];
    [transport setHeader:@"Access-Control-Max-Age" value:@"86400"];
    
    if ([[transport postVars] objectForKey:@"dataXML"]) {
        NSString *error = nil;;
        if (self.debug) NSLog(@"... Input data transmitted as a post variable (dataXML) string. Parsing.");
        if ((error = [self parseAndValidateInputXMLAsString:[[transport postVars] objectForKey:@"dataXML"] orAsData:nil])) {
            [transport setHttpStatusCode:400];
            [transport writeFormat:@"<p>MELTS WS: Error in parsing of XML string form data: %@</p>", error];
            if (self.debug) NSLog(@"... ERROR. Cannot parse input XML. Return status 400.");
            return self;
        }
        if (self.debug) NSLog(@"... Data parsed. XML Document is valid.");
        
    } else if ([[transport postVars] objectForKey:@"dataJSON"]) {
        if (self.debug) NSLog(@"... Input data transmitted as a post variable (dataJSON) string. Parsing.");
        [transport setHttpStatusCode:415];
        [transport write:@"<p>MELTS WS: Parsing of JSON input is not yet supported.</p>"];
        if (self.debug) NSLog(@"... ERROR. Cannot parse input JSON. Return status 415.");
        return self;
        
    } else if ([transport rawPostData] && [[[transport serverVars] objectForKey:@"CONTENT_TYPE"] hasPrefix:@"text/xml"]) {
        NSString *error = nil;;
        if (self.debug) NSLog(@"... Input data transmitted as a post variable (dataXML) string.");
        if ((error = [self parseAndValidateInputXMLAsString:nil orAsData:[transport rawPostData]])) {
            [transport setHttpStatusCode:400];
            [transport writeFormat:@"<p>MELTS WS: Error in parsing of XML string form data: %@</p>", error];
            if (self.debug) NSLog(@"... ERROR. Cannot parse input XML. Return status 400.");
            return self;
        }
        if (self.debug) NSLog(@"... Data parsed. XML Document is valid.");
        
    } else {
        [transport setHttpStatusCode:415];
        [transport write:@"<p>MELTS WS: Unrecognizable or unspecified input.</p>"];
        if (self.debug) NSLog(@"... ERROR. Unrecognizable or unspecified input. Return status 415.");
        return self;
    }
    
    // Version check
    {
        NSError *err;
        NSArray *levelOneChildrenWithSessionID = [[self inputXML] nodesForXPath:@".//title" error:&err];
        if (levelOneChildrenWithSessionID && ([levelOneChildrenWithSessionID count] > 0)) {
            NSString *title = [(NSXMLElement *)[levelOneChildrenWithSessionID objectAtIndex:0] stringValue];
            NSRange range = [title rangeOfString:@"(version "];
            if (range.location != NSNotFound) {
                NSString *versionString = [title substringFromIndex:range.location+range.length];
                NSArray *version = [versionString componentsSeparatedByCharactersInSet:[NSCharacterSet characterSetWithCharactersInString:@".)"]];
                if ([version count] > 2) {
                    NSInteger major = [[version objectAtIndex:0] integerValue];
                    NSInteger minor = [[version objectAtIndex:1] integerValue];
                    NSInteger micro = [[version objectAtIndex:2] integerValue];
                    if (self.debug) NSLog(@"... Excel workbook version:%lu, %lu, %lu", major, minor, micro);
                    if ((major < 1) || (minor < 0) || (micro < 3)) {
                        NSXMLDocument *outputXMLDocument = [[NSXMLDocument alloc] init];
                        NSXMLElement *root = [[NSXMLElement alloc] initWithName:@"MELTSoutput"];
                        [root addChild:[[NSXMLElement alloc] initWithName:@"status" stringValue:@"upgrade"]];
                        [outputXMLDocument setRootElement:root];
                        [transport setHeader:@"Content-type" value:@"text/xml"];
                        [transport writeData:[outputXMLDocument XMLData]];
                        if (self.debug) NSLog(@"... Return XML output.");
                        if (self.debug) NSLog(@"<*><*><*>BaseProcessor Class:(renderWithTransport). Exit status 200.");
                        return self;
                    }
                }
            }
        }
    }
    
    [self setRespondUsingJSON:NO];
    if ([[transport postVars] objectForKey:@"returnJSON"]) [self setRespondUsingJSON:YES];
    
    NSString *sessionId = [[transport cookies] objectForKey:@"sessionid"];
    if (!sessionId) {
        NSError *err;
        NSArray *levelOneChildrenWithSessionID = [[self inputXML] nodesForXPath:@".//sessionID" error:&err];
        if (levelOneChildrenWithSessionID && ([levelOneChildrenWithSessionID count] > 0)) {
            sessionId = [(NSXMLElement *)[levelOneChildrenWithSessionID objectAtIndex:0] stringValue];
            if (self.debug) NSLog(@"... Obtained session Id from XML Input: %@", sessionId);
        }
    } else if (self.debug) NSLog(@"... Obtained session Id from cookie: %@", sessionId);
    
    if ([[[[self inputXML] rootElement] elementsForName:@"initialize"] count] > 0) {
        if (sessionId) [[[self app] state] removeObjectForKey:sessionId];
        sessionId = nil;
        if (self.debug) NSLog(@"... Found an initialize tag and negated the session Id.");
    }
    
    // Lock thread from here to ...
    if (self.debug) NSLog(@"... LOCKED THREAD");
    [(AppDelegate *)[self app] lockState];
    
    rMELTSframework *melts = nil;
    if (!sessionId) {
        sessionId = [NSString stringWithFormat:@"%f.%ld", [NSDate timeIntervalSinceReferenceDate], random()];
        [transport setCookie:@"sessionid" value:sessionId];
        melts = [[rMELTSframework alloc] init];
        [[[self app] state] setObject:melts forKey:sessionId];
    } else melts = [[[self app] state] objectForKey:sessionId];
    if (self.debug) NSLog(@"... MELTS framework object retrieved/assigned to session Id: %@", sessionId);
    
    NSUInteger calculationMode = [melts parseAndLoadDataStructuresFromXMLDocument:[self inputXML]];
    if (self.debug) NSLog(@"... MELTS framework initialized with calculation mode: %lu.", calculationMode);
    
    //
    // This is preliminary code that deals ONLY with possible temperature incremenmts
    // ... It must be expanded to also deal with pressure, volume, enthalpy and entropy increments
    // ... Functions in rMELTSframework are already in place to support this logic
    
    double tInitial   = [melts initialTemperature];
    double tFinal     = [melts finalTemperature];
    double tIncrement = [melts incrementTemperature];
    
    [melts setFinalTemperature:tInitial];
    [melts setIncrementTemperature:0.0];
    Boolean continueLoop = (tIncrement == 0.0) ? NO : YES;
    NSMutableArray *meltsSteps = [NSMutableArray arrayWithCapacity:1];
    
    if (self.debug) NSLog(@"... MELTS input: T initial: %lf, T final: %lf, Tincrement: %lf", tInitial, tFinal, tIncrement);
    do {
        if (tInitial == tFinal) continueLoop = NO;
        @try {
            if ([melts performMELTScalculation:calculationMode] && self.debug) NSLog(@"... MELTS call returned: success.");
            else if (self.debug)                                               NSLog(@"... MELTS call returned: failure.");
            
            [meltsSteps addObject:[melts writeDataStructuresToXMLDocument:sessionId]];
            
            if (tFinal < tInitial) {
                tInitial -= tIncrement;
                if (tFinal > tInitial) tInitial = tFinal;
            } else {
                tInitial += tIncrement;
                if (tFinal < tInitial) tInitial = tFinal;
            }
            
            [melts setInitialTemperature:tInitial];
            [melts setFinalTemperature:tInitial];
        }
        @catch (NSException *exception) {
            continueLoop = NO;
            if (self.debug) {
                NSLog(@"... MELTS framework encountered an exception: %@.", [exception description]);
                NSLog(@"... Removing MELTS framework object for session ID: %@.", sessionId);
            }
            [[[self app] state] removeObjectForKey:sessionId];
        }
    } while (continueLoop);
    
    // End of preliminary code
    //
    
    [(AppDelegate *)[self app] unloackState];
    if (self.debug) NSLog(@"... UNLOCKED THREAD");
    // ... to here
    
    NSXMLDocument *outputXML = nil;
    if ([meltsSteps count] == 1) outputXML = [meltsSteps objectAtIndex:0];
    else {
        outputXML = [[NSXMLDocument alloc] init];
        NSXMLElement *root = [[NSXMLElement alloc] initWithName:@"MELTSsequence"];
        for (NSXMLDocument *step in meltsSteps) {
            [root addChild:[[step rootElement] copy]];
        }
        [outputXML setRootElement:root];
    }
    
    if (![self respondUsingJSON]) {
        [transport setHeader:@"Content-type" value:@"text/xml"];
        [transport writeData:[outputXML XMLData]];
        if (self.debug) NSLog(@"... Return XML output.");
        
    } else {
        if (self.debug) NSLog(@"... Transform XML to JSON.");
        NSError *err = nil;
        NSString *xsltPath = [[NSBundle mainBundle] pathForResource:@"xml2json" ofType:@"xslt"];
        if (!xsltPath) {
            [transport setHttpStatusCode:500];
            [transport write:@"<p>MELTS WS: Internal error in converting XML to JSON.</p>"];
            if (self.debug) NSLog(@"... Error in transforming XML to JSON.");
            if (self.debug) NSLog(@"<*><*><*>BaseProcessor Class:(renderWithTransport). Exit status 500.");
            return self;
        }
        NSData *JSONdata = (NSData *)[outputXML objectByApplyingXSLTAtURL:[NSURL fileURLWithPath:xsltPath]
                                                                arguments:nil  // no extra XSLT parameters needed
                                                                    error:&err];
        if (!JSONdata) {
            [transport setHttpStatusCode:500];
            [transport write:@"<p>MELTS WS: Internal error in converting XML to JSON.</p>"];
            if (self.debug) NSLog(@"<p>... Error - no transformed file.</p>");
            if (self.debug) NSLog(@"<*><*><*>BaseProcessor Class:(renderWithTransport). Exit status 500.");
            return self;
        }
        if (err) {
            [transport setHttpStatusCode:500];
            [transport write:@"<p>MELTS WS: Internal error in converting XML to JSON.</p>"];
            if (self.debug) NSLog(@"<p>... Error: %@.</p>", [err localizedDescription]);
            if (self.debug) NSLog(@"<*><*><*>BaseProcessor Class:(renderWithTransport). Exit status 500.");
            return self;
        }
        
        [transport setHeader:@"Content-type" value:@"application/json"];
        [transport writeData:JSONdata];
        if (self.debug) NSLog(@"... Return JSON output.");
    }
    
    if (self.debug) NSLog(@"<*><*><*>BaseProcessor Class:(renderWithTransport). Exit status 200.");
    return self;
}

@end
