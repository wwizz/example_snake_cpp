package com.lighting.huestream.integrationtests;

import android.util.Log;

import com.lighting.huestream.Area;
import com.lighting.huestream.AreaEffect;
import com.lighting.huestream.Color;
import com.lighting.huestream.ConstantAnimation;
import com.lighting.huestream.Effect;
import com.lighting.huestream.IFeedbackMessageHandler;
import com.lighting.huestream.IterationMode;
import com.lighting.huestream.IterationOrder;
import com.lighting.huestream.LightIteratorEffect;
import com.lighting.huestream.LightSourceEffect;
import com.lighting.huestream.ManualEffect;
import com.lighting.huestream.StreamingMode;
import com.lighting.huestream.TweenAnimation;
import com.lighting.huestream.TweenType;
import com.lighting.huestream.integrationtests.helpers.IBridgeWrapper;
import com.lighting.huestream.integrationtests.helpers.Light;

import org.hamcrest.CoreMatchers;
import org.junit.After;
import org.junit.Assert;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.ErrorCollector;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;

import java.util.ArrayList;
import java.util.List;

@RunWith(JUnit4.class)
public class AnimationTests extends BaseTest {

    private static String RGB_BLACK = "#000000";
    private static int LIGHTS_COUNT = 4;

    private IFeedbackMessageHandler _messageHandler = new FeedBackHandler();

    private Light _frontLeftLight = null;
    private Light _frontRightLight = null;
    private Light _rearLeftLight = null;
    private Light _rearRightLight = null;
    private List<Light> _allLights = null;

    @Rule
    public ErrorCollector _errorCollector = new ErrorCollector();

    @BeforeClass
    public static void loadNativeLibrary() {
        Log.d("IntegrationTests", "Loading native library");
        System.loadLibrary("huestream_java_native");
        Log.d("IntegrationTests", "Loaded native library");
    }

    @Before
    public void setUp() {
        _hue_stream = createStream(StreamingMode.STREAMING_MODE_DTLS);
        _hue_stream.RegisterFeedbackHandler(_messageHandler);

        _bridge = initializeBridge();
        _bridgeWrapperHelper = initializeBridgeWrapper();
        Assert.assertNotNull("_bridgeWrapperHelper is NULL", _bridgeWrapperHelper);
        initializeBridgeResources();

        _hue_stream.ConnectManualBridgeInfo(_bridge);

        threadWaitFor(100);
        expectSameColor(RGB_BLACK, _frontLeftLight, _frontRightLight, _rearLeftLight, _rearRightLight);
    }

    @After
    public void tearDown() {
        threadWaitFor(1000);
        _bridgeWrapperHelper.cleanUpUser();
        _hue_stream.ShutDown();
    }

    private void initializeBridgeResources() {
        Integer entertainmentGroupId = _bridgeWrapperHelper.getEntertainmentGroupId();
        List<IBridgeWrapper.ILightID> lights = _bridgeWrapperHelper.getLLCLightsIDs();

        Assert.assertTrue(lights.size() >= LIGHTS_COUNT);
        if (lights.size() > LIGHTS_COUNT) {
            lights = lights.subList(0, LIGHTS_COUNT);
        }

        initializeLights(lights, entertainmentGroupId);
        _bridge.SelectGroup(entertainmentGroupId.toString());
    }

    private void initializeLights(List<IBridgeWrapper.ILightID> lights, int entertainmentGroupId) {
        Assert.assertEquals("Amount of lights is not equal to LIGHTS_COUNT", LIGHTS_COUNT, lights.size());

        _frontLeftLight = new Light(Light.Position.FrontLeft, lights.get(0));
        _frontRightLight = new Light(Light.Position.FrontRight, lights.get(1));
        _rearLeftLight = new Light(Light.Position.RearLeft, lights.get(2));
        _rearRightLight = new Light(Light.Position.RearRight, lights.get(3));

        _allLights = new ArrayList<>();
        _allLights.add(_frontRightLight);
        _allLights.add(_frontLeftLight);
        _allLights.add(_rearRightLight);
        _allLights.add(_rearLeftLight);


        _bridgeWrapperHelper.includeLightsIntoGroup(lights, entertainmentGroupId);
        _bridgeWrapperHelper.setLightsCoordinates(entertainmentGroupId, lightsAsLightCoordinates());
    }

    private List<IBridgeWrapper.ILightCoordinate> lightsAsLightCoordinates() {
        List<IBridgeWrapper.ILightCoordinate> result = new ArrayList<>();
        for (Light ligth: _allLights) {
            result.add(ligth.asLightCoordinate());
        }

        return result;
    }

    private void addEffectToEngine(Effect... effects) {
        _hue_stream.LockMixer();

        try {
            for (int i = 0; i < effects.length; ++i) {
                _hue_stream.AddEffect(effects[i]);
            }
        } catch (Exception e) {
            Assert.fail("Could not add effect to the engine");
        }

        _hue_stream.UnlockMixer();
    }

    private void expectSameColor(final String expectedColor, Light... lights) {
        Assert.assertNotEquals("Incoming lights collection is empty", 0, lights.length);

        for (Light light : lights) {
            final String actualColor = _bridgeWrapperHelper.getLightRBGColor(light.asLightID());

            _errorCollector.checkThat(String.format("Colors are not equal, expected=%s, actual %s", expectedColor, actualColor),
                    actualColor, CoreMatchers.equalTo(expectedColor));
        }
    }

    private void expectDifferentColor(final String expectedColor, Light... lights) {
        Assert.assertNotEquals("Incoming lights collection is empty", 0, lights.length);

        for (Light light : lights) {
            final String actualColor = _bridgeWrapperHelper.getLightRBGColor(light.asLightID());

            _errorCollector.checkThat(String.format("Colors are equal, not expected=%s, actual %s", expectedColor, actualColor),
                    actualColor, CoreMatchers.not(expectedColor));
        }
    }

    @Test
    public void simpleOverallAreaEffect() {
        final AreaEffect effect = new AreaEffect("simpleAreaEffect", 0);
        effect.AddArea(Area.getAll());
        final Color effectColor = new Color(0.0, 1.0, 0.0);
        effect.SetFixedColor(effectColor);

        addEffectToEngine(effect);
        effect.Enable();

        threadWaitFor(100);
        expectDifferentColor(RGB_BLACK, _frontRightLight, _frontLeftLight, _rearLeftLight, _rearRightLight);
    }

    @Test
    public void twoAreaEffect() {
        final AreaEffect rearLeftPart = new AreaEffect("rearLeft", 1);
        rearLeftPart.AddArea(Area.getBackLeft());
        final Color rearLeftColor = new Color(0.0, 1.0, 0.0);
        rearLeftPart.SetFixedColor(rearLeftColor);

        final AreaEffect frontRightPart = new AreaEffect("frontRight", 2);
        frontRightPart.AddArea(Area.getFrontRight());
        final Color frontRightColor = new Color(1.0, 1.0, 1.0);
        frontRightPart.SetFixedColor(frontRightColor);

        addEffectToEngine(frontRightPart);
        addEffectToEngine(rearLeftPart);
        rearLeftPart.Enable();
        frontRightPart.Enable();

        threadWaitFor(200);
        expectDifferentColor(RGB_BLACK, _rearLeftLight, _frontRightLight);
        expectSameColor(RGB_BLACK, _rearRightLight, _frontLeftLight);
    }

    @Test
    public void lightSourceEffect() {
        final LightSourceEffect effect = new LightSourceEffect();
        effect.SetColorAnimation(new ConstantAnimation(0.0)
                , new ConstantAnimation(1.)
                , new ConstantAnimation(0.0));
        effect.SetPositionAnimation(new ConstantAnimation(0), new ConstantAnimation(0));
        effect.SetRadius(new ConstantAnimation(1.5));

        addEffectToEngine(effect);
        effect.Enable();

        threadWaitFor(100);

        expectDifferentColor(RGB_BLACK, _frontLeftLight, _frontRightLight, _rearLeftLight, _rearRightLight);
    }

    @Test
    public void lightIteratorEffect() {
        int OFFSET_DIRATION = 1000;

        final LightIteratorEffect effect = new LightIteratorEffect();
        effect.SetColorAnimation(new TweenAnimation(0, 0, OFFSET_DIRATION, TweenType.Linear)
                , new TweenAnimation(1, 1, OFFSET_DIRATION, TweenType.Linear)
                , new TweenAnimation(0, 0, OFFSET_DIRATION, TweenType.Linear));

        effect.SetOpacityAnimation(new TweenAnimation(1,1, OFFSET_DIRATION, TweenType.Linear));
        effect.SetOffset(OFFSET_DIRATION);
        effect.SetOrder(IterationOrder.IterationOrderClockwise);
        effect.setMode(IterationMode.IterationModeCycle);

        addEffectToEngine(effect);
        effect.Enable();

        threadWaitFor(100);
        expectDifferentColor(RGB_BLACK, _rearRightLight);
        expectSameColor(RGB_BLACK, _frontRightLight, _frontLeftLight, _rearLeftLight);

        threadWaitFor(OFFSET_DIRATION);
        expectDifferentColor(RGB_BLACK, _rearLeftLight);
        expectSameColor(RGB_BLACK, _rearRightLight, _frontRightLight, _frontLeftLight);


        threadWaitFor(OFFSET_DIRATION);
        expectDifferentColor(RGB_BLACK, _frontLeftLight);
        expectSameColor(RGB_BLACK, _frontRightLight, _rearRightLight, _rearLeftLight);

        threadWaitFor(OFFSET_DIRATION);
        expectDifferentColor(RGB_BLACK, _frontRightLight);
        expectSameColor(RGB_BLACK, _frontLeftLight, _rearLeftLight, _rearRightLight);
    }

    @Test
    public void manualEffect() {
        final ManualEffect effect = new ManualEffect();

        effect.SetIdToColor(_frontLeftLight.getLightId().toString(), new Color(1.0, 0.0, 0.0));
        effect.SetIdToColor(_frontRightLight.getLightId().toString(), new Color(0.0, 1.0, 0.0));
        effect.SetIdToColor(_rearLeftLight.getLightId().toString(), new Color(0.0, 0.0, 1.0));
        effect.SetIdToColor(_rearRightLight.getLightId().toString(), new Color(1.0, 1.0, 0.0));

        addEffectToEngine(effect);
        effect.Enable();

        threadWaitFor(100);

        expectDifferentColor(RGB_BLACK, _frontLeftLight, _frontRightLight, _rearLeftLight, _rearRightLight);
    }
}
