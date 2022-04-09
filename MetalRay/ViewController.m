//
//  ViewController.m
//  MetalRay
//
//  Created by Toby on 4/8/22.
//

#import "ViewController.h"
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>

@implementation ViewController {
    CAMetalLayer *metalLayer;
    id<MTLDevice> device;
    id<MTLLibrary> library;
    id<MTLCommandQueue> queue;
    
    id<MTLRenderPipelineState> blitterPipe;
    id<MTLComputePipelineState> rtPipe;
    
    id<MTLTexture> film;
    id<MTLBuffer> quadVertexBuffer;
    id<MTLSamplerState> mySampler;
    
    NSTimer *timer;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    // Setup resources
    device = MTLCreateSystemDefaultDevice();
    library = [device newDefaultLibrary];
    queue = [device newCommandQueue];
    
    [self.view setWantsLayer:YES];
    
    metalLayer = [CAMetalLayer layer];
    [metalLayer setDevice:device];
    [metalLayer setPixelFormat:MTLPixelFormatBGRA8Unorm];
    [metalLayer setFramebufferOnly:YES];
    // [metalLayer setFrame:CGRectMake(0, 0, 1280, 720)];
    [metalLayer setFrame: self.view.bounds];
    self.view.layer = metalLayer;
    
    MTLRenderPipelineDescriptor *renderPipeDesc = [[MTLRenderPipelineDescriptor alloc] init];
    [renderPipeDesc setVertexFunction:[library newFunctionWithName:@"quadVertex"]];
    [renderPipeDesc setFragmentFunction:[library newFunctionWithName:@"blitterFrag"]];
    renderPipeDesc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    blitterPipe = [device newRenderPipelineStateWithDescriptor:renderPipeDesc error:nil];
    
    rtPipe = [device newComputePipelineStateWithFunction:[library newFunctionWithName:@"rtDraw"] error:nil];
    
    // Setup resources
    MTLTextureDescriptor *texDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm width:1280 height:720 mipmapped:YES];
    texDesc.usage = MTLTextureUsageShaderWrite | MTLTextureUsageShaderRead;
    film = [device newTextureWithDescriptor:texDesc];
    
    const float quadVertices[] = {
        -1.f, -1.f, 0.f, 1.f,
        1.f,  -1.f, 0.f, 1.f,
        -1.f,  1.f, 0.f, 1.f,
        1.f,   1.f, 0.f, 1.f,
    };
    quadVertexBuffer = [device newBufferWithBytes:&quadVertices length:sizeof(quadVertices) options:0];
    
    MTLSamplerDescriptor *samplerDesc = [MTLSamplerDescriptor new];
    samplerDesc.sAddressMode = MTLSamplerAddressModeClampToEdge;
    samplerDesc.tAddressMode = MTLSamplerAddressModeClampToEdge;
    samplerDesc.minFilter = MTLSamplerMinMagFilterLinear;
    samplerDesc.magFilter = MTLSamplerMinMagFilterLinear;
    samplerDesc.mipFilter = MTLSamplerMipFilterLinear;
    samplerDesc.maxAnisotropy = 16;
    mySampler = [device newSamplerStateWithDescriptor:samplerDesc];
    
    timer = [NSTimer scheduledTimerWithTimeInterval:1.f / 24.f target:self selector:@selector(timerCallback) userInfo:nil repeats:YES];
}

- (void)viewWillLayout {
    [metalLayer setDrawableSize:self.view.bounds.size];
}

- (void)timerCallback {
    id<CAMetalDrawable> drawable = [metalLayer nextDrawable];
    
    // Launch the ray tracing kernel
    id<MTLCommandBuffer> cmdBuffer = [queue commandBuffer];
    id<MTLComputeCommandEncoder> computeEncoder = [cmdBuffer computeCommandEncoder];
    [computeEncoder setTexture:film atIndex:0];
    [computeEncoder setComputePipelineState:rtPipe];
    [computeEncoder dispatchThreads:MTLSizeMake(1280, 720, 1) threadsPerThreadgroup:MTLSizeMake(16, 16, 1)];
    [computeEncoder endEncoding];
    
    id<MTLBlitCommandEncoder> blitEncoder = [cmdBuffer blitCommandEncoder];
    [blitEncoder generateMipmapsForTexture:film];
    [blitEncoder endEncoding];
    
    // Blit to backbuffer
    MTLRenderPassDescriptor *renderPass = [MTLRenderPassDescriptor new];
    renderPass.colorAttachments[0].texture = drawable.texture;
    renderPass.colorAttachments[0].clearColor = MTLClearColorMake(0., 0., 0., 0.);
    renderPass.colorAttachments[0].loadAction = MTLLoadActionClear;
    id<MTLRenderCommandEncoder> renderEncoder = [cmdBuffer renderCommandEncoderWithDescriptor:renderPass];
    [renderEncoder setRenderPipelineState:blitterPipe];
    [renderEncoder setVertexBuffer:quadVertexBuffer offset:0 atIndex:0];
    [renderEncoder setFragmentTexture:film atIndex:0];
    [renderEncoder setFragmentSamplerState:mySampler atIndex:0];
    [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangleStrip vertexStart:0 vertexCount:4];
    [renderEncoder endEncoding];
    
    [cmdBuffer presentDrawable:drawable];
    [cmdBuffer commit];
}

- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}


@end
