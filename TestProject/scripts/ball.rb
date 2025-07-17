# frozen_string_literal: true

# Game ball
class Ball < Borek::Scriptable
  def initialize(entity)
    @velocity = Borek::Vec2.new(1, 1)
    @speed = 100
    super entity
  end

  def check_collision(paddle)
    paddle_transform = paddle.transform
    paddle_position = paddle_transform.position
    paddle_scale = paddle_transform.scale

    diff = Borek::Vec2.new((transform.position.x - paddle_position.x).abs,
                           (transform.position.y - paddle_position.y).abs)

    (diff.x < (paddle_scale.x / 2)) &&
      ((diff.y) < (paddle_scale.y / 2))
  end

  def on_update(delta)
    @velocity.y = @velocity.y * -1 if transform.position.y.abs > 190

    transform.position.x += @velocity.x * @speed * delta
    transform.position.y += @velocity.y * @speed * delta

    paddle_l = Borek::Scene.get_child('Player 1')
    paddle_r = Borek::Scene.get_child('Player 2')

    if check_collision(paddle_l)
      @velocity.x = 1
      @speed += 10
    elsif check_collision(paddle_r)
      @velocity.x = -1
      @speed += 10
    end

    Borek::Log.info("FPS: #{1 / delta}")
  end
end
